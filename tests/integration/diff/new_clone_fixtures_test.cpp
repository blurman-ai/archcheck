// Fixture-driven acceptance for the --diff new-clone gate (#123). Each scenario
// dir under fixtures/diff_new_clone/ holds a parent/ tree, a new/ tree, and an
// added.txt declaring the commit's added-line ranges. The test reads both trees
// as snapshots and drives detectNewClones — the same call diff_command.cpp makes
// — so the documented fixture shape is exercised, not just the synthetic E2E.

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "archcheck/scan/disk_file_source.h"
#include "archcheck/scan/new_clone_drift.h"
#include "archcheck/scan/source_snapshot.h"

namespace
{

namespace fs = std::filesystem;
using archcheck::scan::AddedLineMap;
using archcheck::scan::detectNewClones;
using archcheck::scan::DiskFileSource;
using archcheck::scan::SourceSnapshot;

fs::path scenarioDir(std::string_view name) { return fs::path{ARCHCHECK_FIXTURES_DIR} / "diff_new_clone" / name; }

// Parse added.txt: one "<path>:<start>-<end>" (or "<path>:<line>") per line,
// '#'-comments and blanks ignored.
AddedLineMap loadAddedLines(const fs::path &file)
{
  AddedLineMap added;
  std::ifstream in(file);
  REQUIRE(in.is_open());
  std::string line;
  while (std::getline(in, line))
  {
    if (line.empty() || line[0] == '#')
      continue;
    const auto colon = line.find(':');
    REQUIRE(colon != std::string::npos);
    const std::string path = line.substr(0, colon);
    const std::string range = line.substr(colon + 1);
    const auto dash = range.find('-');
    const int start = std::stoi(range.substr(0, dash));
    const int end = dash == std::string::npos ? start : std::stoi(range.substr(dash + 1));
    for (int ln = start; ln <= end; ++ln)
      added[path].insert(ln);
  }
  return added;
}

archcheck::scan::NewCloneDriftResult runScenario(std::string_view name)
{
  const auto dir = scenarioDir(name);
  DiskFileSource parentSrc(dir / "parent");
  DiskFileSource newSrc(dir / "new");
  const auto added = loadAddedLines(dir / "added.txt");
  return detectNewClones(SourceSnapshot::read(newSrc), SourceSnapshot::read(parentSrc), added);
}

} // namespace

TEST_CASE("diff_new_clone fixture: function copied into an existing file fires", "[diff][newclone][fixtures]")
{
  const auto res = runScenario("fail_existing_file");
  REQUIRE(res.violations.size() >= 1);
  REQUIRE(res.violations[0].ruleId == "DRIFT.NEW_CLONE");
  REQUIRE(res.violations[0].file == "helper.cpp");
  REQUIRE(res.violations[0].message.find("clone of orig.cpp") != std::string::npos);
}

TEST_CASE("diff_new_clone fixture: a whole-file copy added fires", "[diff][newclone][fixtures]")
{
  const auto res = runScenario("fail_new_file");
  REQUIRE(res.violations.size() >= 1);
  REQUIRE(res.violations[0].ruleId == "DRIFT.NEW_CLONE");
  REQUIRE(res.violations[0].file == "copy.cpp");
  REQUIRE(res.violations[0].message.find("clone of orig.cpp") != std::string::npos);
}

TEST_CASE("diff_new_clone fixture: unique new code stays silent", "[diff][newclone][fixtures]")
{
  REQUIRE(runScenario("pass").violations.empty());
}

TEST_CASE("diff_new_clone fixture: pre-existing clone merely touched stays silent", "[diff][newclone][fixtures]")
{
  REQUIRE(runScenario("pass_preexisting").violations.empty());
}

TEST_CASE("diff_new_clone fixture: whole-subtree byte-copy mount is suppressed (#179 follow-up)",
          "[diff][newclone][fixtures]")
{
  // A commit re-mounts an existing MULTI-fragment lib file under a new prefix (base-lib
  // mount / cross-mount byte-copy). Two-sided whole-file suppression (lo>=2) must drop it.
  REQUIRE(runScenario("mount_copy").violations.empty());
}

TEST_CASE("diff_new_clone fixture: small module extracted into a new file still fires", "[diff][newclone][fixtures]")
{
  // Guardrail that whole-file suppression does not over-reach: a one-sided small (1 frag)
  // extracted out of a large (3 frag) file is NOT a whole-file twin and stays reported.
  const auto res = runScenario("extraction");
  REQUIRE(res.violations.size() >= 1);
  REQUIRE(res.violations[0].file == "extracted.cpp");
  REQUIRE(res.violations[0].message.find("clone of big.cpp") != std::string::npos);
}
