#pragma once

#include <filesystem>
#include <optional>

#include "archcheck/config/config.h"

namespace archcheck::cli
{

enum class OutputFormat
{
  Text,
  Json,
  Markdown
};

enum class BaselineMode
{
  None,
  Load,
  Save
};

struct BaselineOpts
{
  BaselineMode mode = BaselineMode::None;
  std::filesystem::path file;
  std::optional<std::filesystem::path> driftFile; // --drift-baseline <file>
};

// Run all default rules on `root` and report. Owns baseline filtering and
// delegates check/drift exit gating to rules/gate_policy.h.
// Discovers .archcheck.yml from `root` unless an explicit `config` is given.
// `failOnUnresolvedLocal` (--fail-on-unresolved-local) promotes advisory
// UNRESOLVED_LOCAL_INCLUDE findings to gating; CASE_MISMATCH_INCLUDE always gates.
int runCheck(const std::filesystem::path &root, OutputFormat fmt, BaselineOpts baseline = {},
             std::optional<config::Config> config = std::nullopt, bool failOnUnresolvedLocal = false);

// Save the include-graph snapshot of `root` for later --drift-baseline runs.
int runSaveGraphBaseline(const std::filesystem::path &root, const std::filesystem::path &file);

// Apply a loaded config's additive classification overrides (#154 Phase 2) to the
// scan layer's set-once registry. Call ONCE before scanning; in --diff this must
// run before either side is read so baseline and current stay consistent.
void applyClassificationConfig(const config::Config &config);

} // namespace archcheck::cli
