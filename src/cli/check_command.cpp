#include "cli/check_command.h"

#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "archcheck/config/config_loader.h"
#include "archcheck/graph/baseline.h"
#include "archcheck/graph/graph_builder.h"
#include "archcheck/report/json_reporter.h"
#include "archcheck/report/text_reporter.h"
#include "archcheck/report/violation_baseline.h"
#include "archcheck/rules/gate_policy.h"
#include "archcheck/rules/rule_set.h"
#include "archcheck/scan/disk_file_source.h"
#include "archcheck/scan/file_classification.h"
#include "archcheck/scan/local_include_scan.h"
#include "archcheck/scan/source_snapshot.h"

namespace archcheck::cli
{

namespace
{

// Returns 0 on success, 2 on I/O error (already printed to stderr).
int trySaveBaseline(const archcheck::rules::ViolationList &all, const std::filesystem::path &file)
{
  try
  {
    archcheck::report::saveBaseline({all}, file);
  }
  catch (const archcheck::report::BaselineError &e)
  {
    std::cerr << "archcheck: " << e.what() << '\n';
    return 2;
  }
  std::cout << "baseline saved: " << all.size() << " violation(s) → " << file.string() << '\n';
  return 0;
}

// Filters `all` in-place. Returns suppressed count, or -1 on error (already printed to stderr).
int tryLoadAndFilter(archcheck::rules::ViolationList &all, const std::filesystem::path &file)
{
  try
  {
    const auto b = archcheck::report::loadBaseline(file);
    const auto filtered = archcheck::report::filterNew(all, b);
    const int suppressed = static_cast<int>(all.size() - filtered.size());
    all = filtered;
    return suppressed;
  }
  catch (const archcheck::report::BaselineError &e)
  {
    std::cerr << "archcheck: " << e.what() << '\n';
    return -1;
  }
}

// stdout colorization: on only for an interactive TTY with NO_COLOR unset
// (https://no-color.org). Piped/redirected output stays plain for CI logs.
bool textReportUseColor()
{
  const char *noColor = std::getenv("NO_COLOR");
  if (noColor != nullptr && noColor[0] != '\0')
    return false;
  return ::isatty(fileno(stdout)) != 0;
}

// Drift mode is a regression gate: DRIFT.1 (new shortcut edge), DRIFT.2
// (new/grown cycle), and DRIFT.4.CYCLE (lateral module cycle) gate the exit.
// Pre-existing intrinsic findings (SF.*/Lakos.*) and advisory signals
// (DRIFT.3, DRIFT.4.SDP, DRIFT.4.NEW) are reported but never fail the run.
int reportDriftGate(const archcheck::rules::ViolationList &all, OutputFormat fmt)
{
  const auto gating = archcheck::rules::countGating(all, archcheck::rules::GateMode::Drift);
  if (fmt == OutputFormat::Text)
    std::cout << "drift gate: " << gating
              << " gating regression(s) (DRIFT.1/DRIFT.2/DRIFT.4.CYCLE);"
                 " pre-existing and advisory findings (DRIFT.3/DRIFT.4.SDP/DRIFT.4.NEW) are advisory\n";
  return gating > 0 ? 1 : 0;
}

// Plain check mode gates (exit 1) only on the architectural regression the tool is
// most precise on: a dependency cycle (SF.9). Physical-design proxies
// (Lakos.ChainLength deep include chains, Lakos.GodHeader fan-in) and per-file
// hygiene (SF.7/SF.8) are reported but advisory — they flood header-heavy libraries
// on a naive first run (abseil: 211 chain-length, exit 1) and existing debt belongs
// behind --baseline, not a hard exit. This mirrors the narrow-gate trust model,
// but not the exact --diff gate: diff also gates new god-headers. #133.
int reportCheckGate(const archcheck::rules::ViolationList &all, OutputFormat fmt, bool failOnUnresolvedLocal)
{
  const auto gating = archcheck::rules::countGating(all, archcheck::rules::GateMode::Check, failOnUnresolvedLocal);
  const auto advisory = all.size() - static_cast<std::size_t>(gating);
  if (fmt == OutputFormat::Text && advisory > 0)
    std::cout << "note: " << advisory
              << " advisory finding(s) reported, not gated (chain-length / god-header / SF.7-8 are"
                 " physical-design advisories); the gate fails on dependency cycles (SF.9) and include"
                 " case mismatches (CASE_MISMATCH_INCLUDE). Use --baseline to track existing debt.\n";
  return gating > 0 ? 1 : 0;
}

archcheck::rules::GateMode gateModeFor(const BaselineOpts &baseline)
{
  return baseline.driftFile ? archcheck::rules::GateMode::Drift : archcheck::rules::GateMode::Check;
}

// Locate the enclosing git repo when `root` is a subtree of one (archcheck src).
// Quoted includes then routinely resolve against include roots elsewhere in the
// repo — those files exist, so they must not be reported (#167). Returns nullopt
// when root IS the repo root (the subtree index already covers everything) or no
// repo is found; the scan then runs on the subtree index alone.
std::optional<archcheck::scan::RepoUniverse> repoUniverseFor(const std::filesystem::path &root)
{
  std::error_code ec;
  const std::filesystem::path canonicalRoot = std::filesystem::canonical(root, ec);
  if (ec || std::filesystem::exists(canonicalRoot / ".git"))
    return std::nullopt;
  for (auto dir = canonicalRoot.parent_path(); !dir.empty(); dir = dir.parent_path())
  {
    if (std::filesystem::exists(dir / ".git"))
      return archcheck::scan::RepoUniverse{canonicalRoot.lexically_relative(dir).generic_string(),
                                           archcheck::scan::discoverFiles(dir)};
    if (dir == dir.parent_path())
      break;
  }
  return std::nullopt;
}

// Local include-resolution findings (#167): a quoted include that resolves only
// under a different filesystem case is a portability break (gating by default);
// one that resolves not at all is advisory (may be generated / custom root).
archcheck::rules::Violation toViolation(const archcheck::scan::LocalIncludeIssue &issue)
{
  if (issue.kind == archcheck::scan::LocalIncludeIssue::Kind::CaseMismatch)
    return {"CASE_MISMATCH_INCLUDE", issue.file, issue.line,
            "quoted include \"" + issue.include + "\" resolves only case-insensitively to " +
                issue.resolvedCaseInsensitive + " (breaks on case-sensitive filesystems)"};
  return {"UNRESOLVED_LOCAL_INCLUDE", issue.file, issue.line,
          "quoted local include \"" + issue.include + "\" does not resolve under project roots"};
}

void writeReport(const archcheck::rules::ViolationList &all, OutputFormat fmt, const BaselineOpts &baseline,
                 bool failOnUnresolvedLocal)
{
  if (fmt == OutputFormat::Json)
    archcheck::report::writeJsonReport(all, std::cout, gateModeFor(baseline), failOnUnresolvedLocal);
  else
    archcheck::report::writeTextReport(all, std::cout, textReportUseColor());
}

int applyBaselineAndReport(archcheck::rules::ViolationList all, OutputFormat fmt, const BaselineOpts &baseline,
                           bool failOnUnresolvedLocal)
{
  if (baseline.mode == BaselineMode::Save)
    return trySaveBaseline(all, baseline.file);

  std::size_t suppressed = 0;
  if (baseline.mode == BaselineMode::Load)
  {
    const int n = tryLoadAndFilter(all, baseline.file);
    if (n < 0)
      return 2;
    suppressed = static_cast<std::size_t>(n);
  }

  writeReport(all, fmt, baseline, failOnUnresolvedLocal);

  if (suppressed > 0 && fmt == OutputFormat::Text)
    std::cout << "suppressed: " << suppressed << " known violation(s) (run without --baseline to see all)\n";

  if (baseline.driftFile)
    return reportDriftGate(all, fmt);

  return reportCheckGate(all, fmt, failOnUnresolvedLocal);
}

int applyDriftFile(const std::filesystem::path &driftFile, std::vector<std::unique_ptr<archcheck::rules::IRule>> &rules)
{
  std::ifstream in(driftFile);
  if (!in)
  {
    std::cerr << "archcheck: cannot open drift baseline: " << driftFile.string() << '\n';
    return 2;
  }
  auto [g, err] = archcheck::graph::loadBaseline(in);
  if (err)
  {
    std::cerr << "archcheck: drift baseline error: " << err->message << '\n';
    return 2;
  }
  for (auto &r : archcheck::rules::makeDriftRuleSet(std::move(g)))
    rules.push_back(std::move(r));
  return 0;
}

// Bridge the loaded config's additive classification overrides into the scan
// layer's set-once registry (#154 Phase 2). Dir tokens are normalized / markers
// lowercased to match how the predicates compare.
archcheck::scan::ClassificationExtras toClassificationExtras(const config::Classification &c)
{
  archcheck::scan::ClassificationExtras extras;
  for (const auto &dir : c.extraVendoredDirs)
    extras.vendoredDirs.insert(archcheck::scan::normalizeDirSegment(dir));
  for (const auto &dir : c.extraTestDirs)
    extras.testDirs.insert(archcheck::scan::normalizeDirSegment(dir));
  for (const auto &marker : c.extraGeneratedMarkers)
    extras.generatedMarkers.insert(archcheck::scan::toLowerAscii(marker));
  return extras;
}

} // namespace

void applyClassificationConfig(const config::Config &cfg)
{
  archcheck::scan::setClassificationExtras(toClassificationExtras(cfg.classification));
}

// Read+classify the tree once (#129); the graph and the SF.* text rules share the
// one snapshot instead of re-reading every header from disk (was up to 3x: graph
// build + SF.7 + SF.8). readFile serves from the in-memory snapshot.
archcheck::rules::ViolationList checkViolations(const std::filesystem::path &root,
                                                const std::vector<std::unique_ptr<archcheck::rules::IRule>> &rules)
{
  archcheck::scan::DiskFileSource src(root);
  const auto snapshot = archcheck::scan::SourceSnapshot::read(src);
  const auto built = archcheck::graph::buildGraphForSnapshot(snapshot);
  const auto readFile = [&snapshot](std::string_view path) -> std::string
  {
    const archcheck::scan::SnapshotFile *f = snapshot.findFile(path);
    return f != nullptr ? f->content : std::string{};
  };
  archcheck::rules::ViolationList all;
  for (const auto &rule : rules)
  {
    const auto v = rule->check(built.graph, readFile);
    all.insert(all.end(), v.begin(), v.end());
  }
  const auto universe = repoUniverseFor(root);
  for (const auto &issue : archcheck::scan::scanLocalIncludeIssues(snapshot, universe ? &*universe : nullptr))
    all.push_back(toViolation(issue));
  return all;
}

int runCheck(const std::filesystem::path &root, OutputFormat fmt, BaselineOpts baseline,
             std::optional<config::Config> config, bool failOnUnresolvedLocal)
{
  if (!config)
  {
    try
    {
      config = archcheck::config::discover(root);
    }
    catch (const archcheck::config::ConfigError &e)
    {
      std::cerr << "archcheck: " << e.what() << '\n';
      return 2;
    }
  }
  applyClassificationConfig(*config);
  auto rules = archcheck::rules::makeDefaultRuleSet(*config);
  const int driftRc = baseline.driftFile ? applyDriftFile(*baseline.driftFile, rules) : 0;
  if (driftRc != 0)
    return driftRc;
  return applyBaselineAndReport(checkViolations(root, rules), fmt, baseline, failOnUnresolvedLocal);
}

int runSaveGraphBaseline(const std::filesystem::path &root, const std::filesystem::path &file)
{
  try
  {
    // Same classification overrides the later --drift-baseline run will use, so the
    // saved snapshot and the current scan are built on a consistent file set (#154 2b).
    applyClassificationConfig(archcheck::config::discover(root));
  }
  catch (const archcheck::config::ConfigError &e)
  {
    std::cerr << "archcheck: " << e.what() << '\n';
    return 2;
  }
  const auto built = archcheck::graph::buildGraphForPath(root);
  std::ofstream out(file);
  if (!out)
  {
    std::cerr << "archcheck: cannot write graph baseline: " << file.string() << '\n';
    return 2;
  }
  archcheck::graph::saveBaseline(built.graph, out);
  std::cout << "graph baseline saved: " << built.graph.nodeCount() << " node(s) \xe2\x86\x92 " << file.string() << '\n';
  return 0;
}

} // namespace archcheck::cli
