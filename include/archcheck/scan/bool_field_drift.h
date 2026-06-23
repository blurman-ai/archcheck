#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "archcheck/rules/violation.h"

namespace archcheck::scan
{

class SourceSnapshot;

// Result of the baseline/current bool-field-accretion comparison for changed files.
// Advisory only: never gates the exit code. A struct that EXISTED in the baseline and
// gained net bool fields is incremental boolean-state drift (#135 metric; constraint
// decay — Martin "Make Illegal States Unrepresentable").
struct BoolFieldDriftResult
{
  rules::ViolationList violations;
};

// Compare depth-0 bool-field counts per struct/class between the two versions of one
// file. A struct present in BOTH versions whose bool-field count grew yields one finding
// (net count: rename / typo-fix / reformat keep the count, so they do not fire). A struct
// absent from oldSource (new file / new struct) is greenfield, not drift, and is skipped.
[[nodiscard]] BoolFieldDriftResult compareBoolFields(const std::string &oldSource, const std::string &newSource,
                                                     const std::string &file);

// Run the comparison over the changed C/C++ files of a diff, taking both versions from the
// pre-read+classified snapshots. A changed file's authored verdict comes from the whole
// current tree, so vendored / test / generated / banner files are dropped with the same
// classification as graph, clone and complexity (#129).
[[nodiscard]] BoolFieldDriftResult detectBoolFieldDrift(const SourceSnapshot &oldSnapshot,
                                                        const SourceSnapshot &newSnapshot,
                                                        const std::vector<std::filesystem::path> &changedFiles);

} // namespace archcheck::scan
