#pragma once

#include <string>
#include <vector>

#include "archcheck/scan/project_files.h"

namespace archcheck::scan
{

class SourceSnapshot;

// A quoted local #include that does not resolve against the project with exact
// filesystem case. Two disjoint kinds (#167):
struct LocalIncludeIssue
{
  enum class Kind
  {
    CaseMismatch, // resolves only when case is ignored — a build break on case-sensitive filesystems
    Unresolved,   // no match at all — may be generated or depend on a custom include root
  };

  Kind kind;
  std::string file;                    // repo-relative source TU holding the directive
  int line;                            // 1-based directive line
  std::string include;                 // the quoted token, verbatim
  std::string resolvedCaseInsensitive; // matched project path; empty for Unresolved
};

// When the scan root is a subtree of a larger repo (`archcheck src`), quoted
// includes routinely resolve against include roots elsewhere in that repo
// (src/cli includes "archcheck/..." living in include/). Those files exist, so
// they are not violations. The universe carries the enclosing repo's file list
// (repo-relative) plus the scan root's prefix inside it: an exact-case match
// there suppresses the finding; a case-insensitive-only match is still a
// CASE_MISMATCH (the build breaks the same way across the boundary).
struct RepoUniverse
{
  std::string prefix;             // scan root's repo-relative path, e.g. "src"
  std::vector<ProjectFile> files; // every project file in the enclosing repo
};

// Scan authored sources for quoted local #includes that fail exact-case
// resolution. Angle-bracket / system includes are never reported: without full
// compiler context they are not project violations. Reuses resolveInclude, so a
// directive that the graph resolves (exact, relative, or suffix) is never flagged.
// Deterministic: results follow snapshot file order, ties broken lexicographically.
std::vector<LocalIncludeIssue> scanLocalIncludeIssues(const SourceSnapshot &snapshot,
                                                      const RepoUniverse *universe = nullptr);

} // namespace archcheck::scan
