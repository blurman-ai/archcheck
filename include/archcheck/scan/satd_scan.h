#pragma once

#include <vector>

#include "archcheck/git/diff_query.h"
#include "archcheck/rules/violation.h"
#include "archcheck/scan/source_snapshot.h"

namespace archcheck::scan
{

// Detect self-admitted technical debt markers in added lines (from unified diff).
// Implements SATD.1 (new SATD marker) and SATD.2 (FIXME/HACK without issue id).
//
// Rules:
//   SATD.1: Any new marker on added line:
//     - UPPERCASE keywords: \b(TODO|FIXME|HACK|XXX|TEMP)\b
//     - case-insensitive: \btemporary\b, \bworkaround\b, \bquick.?fix\b, \bdirty\b
//   SATD.2: FIXME or HACK on added line without issue id:
//     - Issue id patterns: ([A-Z][A-Z0-9]+-\d+)|(#\d+)|(\b(gh|issue)[-/]\d+)
//
// Marker must be in comment part only (after // or inside /* */).
// One violation per line (not per marker).
//
// `newSnapshot` is the child-tree SourceSnapshot: each added line is gated on its
// file's content-aware `authored` verdict (drops SWIG amalgams / license-banner
// vendors that the path-only gate misses), falling back to the path/basename gate
// for files not listed in the snapshot (non-C/C++ extensions).
[[nodiscard]] rules::ViolationList detectSatdMarkers(const std::vector<git::AddedLine> &addedLines,
                                                     const SourceSnapshot &newSnapshot);

} // namespace archcheck::scan
