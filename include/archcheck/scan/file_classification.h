#pragma once

#include <algorithm>
#include <array>
#include <string_view>

// Single source of truth for the file/dir classification defaults shared by
// every file source (disk traversal + git object DB) and the text-scan rules.
// Task #041: keep these embedded defaults in one place instead of duplicating
// the arrays per translation unit (they had already drifted between the disk
// and git backends).
//
// NOTE: vendor / third_party trees are intentionally NOT excluded here —
// discoverFiles must still surface them (test "discover_files does NOT
// auto-exclude third_party / vendor"). Vendor exclusion is a graph/diff-layer
// concern, not a file-enumeration one.
namespace archcheck::scan
{

// Project source + header extensions recognised by the v0.1 scan.
inline constexpr std::array<std::string_view, 12> kProjectExtensions = {
    ".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx", ".ipp", ".tpp", ".inl", ".inc",
};

// Header-only subset; text-scan rules (SF.7, SF.8) apply only to these.
inline constexpr std::array<std::string_view, 8> kHeaderExtensions = {
    ".h", ".hh", ".hpp", ".hxx", ".ipp", ".tpp", ".inl", ".inc",
};

// Directory names skipped during traversal (exact match), plus the
// cmake-build-* prefix handled separately below.
inline constexpr std::array<std::string_view, 6> kExcludedDirNames = {
    ".git", "build", ".cache", ".idea", ".vscode", "out",
};
inline constexpr std::string_view kCmakeBuildPrefix = "cmake-build-";

// True if a single path segment names a directory the scan must not descend
// into. Shared by DiskFileSource (per-entry) and GitObjectFileSource (per path
// segment) so the exclusion set cannot drift between the two backends.
inline bool isExcludedDirName(std::string_view name)
{
  if (std::find(kExcludedDirNames.begin(), kExcludedDirNames.end(), name) != kExcludedDirNames.end())
  {
    return true;
  }
  return name.size() >= kCmakeBuildPrefix.size() && name.compare(0, kCmakeBuildPrefix.size(), kCmakeBuildPrefix) == 0;
}

} // namespace archcheck::scan
