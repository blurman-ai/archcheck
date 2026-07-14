#pragma once

#include <cstddef>
#include <string>
#include <variant>
#include <vector>

namespace archcheck::config
{

struct ModuleDef
{
  std::string name;
  std::vector<std::string> paths;
};

struct LayersRule
{
  std::string name;
  std::vector<std::string> layers;
};

struct IndependenceRule
{
  std::string name;
  std::vector<std::string> modules;
};

struct ForbiddenRule
{
  std::string name;
  std::vector<std::string> from;
  std::vector<std::string> to;
};

using Rule = std::variant<LayersRule, IndependenceRule, ForbiddenRule>;

// Tunable rule defaults, single source of truth. Overridable via the phase-2
// `thresholds:` block in .archcheck.yml (see docs/config_format.md).
struct Thresholds
{
  std::size_t chainLength = 10;    // Lakos.ChainLength: include chain depth
  std::size_t godHeaderFanIn = 50; // Lakos.GodHeader: header fan-in
  // FUNC.COGNITIVE_COMPLEXITY: absolute per-function Sonar Cognitive Complexity
  // ceiling (advisory). 40 is corpus-calibrated to keep this always-on, whole-repo
  // advisory focused on the ~top 3% of functions; the scorer follows Campbell 2018.
  // Authorities set the per-function line lower (clang-tidy 25, SonarQube S3776 15)
  // but those are gates / IDE rules, not a zero-config bulk advisory. Override via
  // thresholds: in .archcheck.yml.
  std::size_t functionComplexity = 40;
  // --diff: skip the local-complexity advisory when the diff adds more lines
  // (bulk source imports are not authored evolution; #109 corpus calibration —
  // genuine large features stay below ~6k, bulk drops start at ~20k).
  std::size_t diffMaxAddedLines = 10000;
  // --diff: skip the new-clone advisory when the authored tree exceeds this many
  // bytes. The clone scan is a whole-tree pass (the twin of an added clone may
  // live in an unchanged file), so its cost is O(tree), not O(diff): a small
  // commit to a huge repo still pays it (#149). Past this size it does not fit the
  // per-commit budget — skipping keeps the GATE (cycles/god-headers) running.
  // ~40 MB authored ≈ 2x apache/arrow (~21 MB, ~4.5 s); giants skip, normal repos
  // keep the advisory. Advisory-only ⇒ never changes the gate.
  std::size_t diffMaxCloneScanBytes = 40 * 1024 * 1024;
};

// Additive overrides for the file/dir classification defaults (#154 Phase 2).
// Project-specific tokens layered ON TOP of the curated embedded defaults in
// scan/file_classification.h — never a replacement. Optional `classification:`
// block in .archcheck.yml; see docs/config_format.md.
struct Classification
{
  std::vector<std::string> extraVendoredDirs;
  std::vector<std::string> extraTestDirs;
  std::vector<std::string> extraGeneratedMarkers;
};

struct Config
{
  int version = 0;
  std::vector<ModuleDef> modules;
  std::vector<Rule> rules;
  Thresholds thresholds;
  Classification classification;
};

} // namespace archcheck::config
