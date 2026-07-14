#pragma once

#include <cstddef>

#include "archcheck/rules/violation.h"
#include "archcheck/scan/source_snapshot.h"

namespace archcheck::scan
{

// Absolute per-function cognitive-complexity check for the standard `archcheck
// [path]` run (advisory, FUNC.COGNITIVE_COMPLEXITY). Reuses the #101 token-level
// scorer (computeFileComplexity) over every authored file and flags each function
// whose Sonar Cognitive Complexity (Campbell 2018) reaches `threshold`. Unlike the
// --diff DRIFT.LOCAL_COMPLEXITY advisory it looks at the current tree, not a delta.
[[nodiscard]] rules::ViolationList detectAbsoluteComplexity(const SourceSnapshot &snapshot, std::size_t threshold);

} // namespace archcheck::scan
