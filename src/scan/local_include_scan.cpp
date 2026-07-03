#include "archcheck/scan/local_include_scan.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "archcheck/scan/include_directive.h"
#include "archcheck/scan/include_resolver.h"
#include "archcheck/scan/include_scanner.h"
#include "archcheck/scan/project_files.h"
#include "archcheck/scan/resolved_include.h"
#include "archcheck/scan/source_snapshot.h"

namespace archcheck::scan
{

namespace
{

using CiSuffixIndex = std::unordered_map<std::string, std::vector<std::string>>;

std::string to_lower(std::string_view s)
{
  std::string out(s);
  for (char &c : out)
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return out;
}

// lowercased '/'-suffix -> project paths that carry it. Mirrors
// ProjectIndex.suffixIndex folded to lower case, so a token whose exact-case
// lookup missed can still be matched ignoring case (that IS the case mismatch).
CiSuffixIndex build_ci_suffix_index(const std::vector<ProjectFile> &files)
{
  CiSuffixIndex index;
  for (const auto &f : files)
  {
    const std::string_view path = f.path;
    for (std::size_t pos = 0; pos != std::string_view::npos;)
    {
      index[to_lower(path.substr(pos))].push_back(f.path);
      const std::size_t slash = path.find('/', pos);
      pos = (slash == std::string_view::npos) ? std::string_view::npos : slash + 1;
    }
  }
  return index;
}

// The subtree index plus (optionally) the enclosing repo's indexes.
struct ScanContext
{
  const std::vector<ProjectFile> &files;
  const ProjectIndex &index;
  const CiSuffixIndex &ciSuffix;
  const RepoUniverse *universe;
  ProjectIndex universeIndex;
  CiSuffixIndex universeCi;
};

// Lexicographically smallest case-insensitive match for the token, if any.
std::optional<std::string> ci_match(const CiSuffixIndex &ci, const std::string &token)
{
  const auto it = ci.find(to_lower(token));
  if (it == ci.end() || it->second.empty())
    return std::nullopt;
  return *std::min_element(it->second.begin(), it->second.end());
}

// True when the token names a file that exists with EXACT case, so it is not a
// case problem. resolveInclude can still return Unresolved for such a token: a
// self-include (`enc_fast_lossless.cc` #including itself for macro re-expansion)
// is downgraded because a component never depends on itself, yet the exact-case
// file plainly exists. We also accept a suffix / rebased-relative match anywhere
// in the enclosing repo (an include root outside the scan subtree). Without this
// guard every self-include is a phantom CASE_MISMATCH against itself (#168).
bool exact_case_exists(const IncludeDirective &d, const std::string &sourceFile, const ScanContext &ctx)
{
  if (ctx.index.suffixIndex.count(d.token) != 0)
    return true;
  if (ctx.universe == nullptr)
    return false;
  if (ctx.universeIndex.suffixIndex.count(d.token) != 0)
    return true;
  const std::size_t slash = sourceFile.rfind('/');
  const std::string sourceDir = (slash == std::string::npos) ? std::string{} : sourceFile.substr(0, slash + 1);
  const std::string rebased =
      std::filesystem::path(ctx.universe->prefix + "/" + sourceDir + d.token).lexically_normal().generic_string();
  return ctx.universeIndex.exactPathIndex.count(rebased) != 0;
}

// A quoted local include the graph could not resolve with exact case becomes an
// issue; anything that exists exact-case (self-include, suffix, or an include
// root elsewhere in the repo) is silent, and only a case-insensitive-only match
// is a genuine CASE_MISMATCH.
std::optional<LocalIncludeIssue> classify(const IncludeDirective &d, const std::string &sourceFile,
                                          const ScanContext &ctx)
{
  if (d.kind != IncludeKind::Quote)
    return std::nullopt;
  if (resolveInclude(d, sourceFile, ctx.files, ctx.index).resolution != Resolution::Unresolved)
    return std::nullopt;
  if (exact_case_exists(d, sourceFile, ctx))
    return std::nullopt;

  if (auto match = ci_match(ctx.ciSuffix, d.token))
    return LocalIncludeIssue{LocalIncludeIssue::Kind::CaseMismatch, sourceFile, d.line, d.token, std::move(*match)};
  if (ctx.universe != nullptr)
    if (auto match = ci_match(ctx.universeCi, d.token))
      return LocalIncludeIssue{LocalIncludeIssue::Kind::CaseMismatch, sourceFile, d.line, d.token, std::move(*match)};
  return LocalIncludeIssue{LocalIncludeIssue::Kind::Unresolved, sourceFile, d.line, d.token, {}};
}

} // namespace

std::vector<LocalIncludeIssue> scanLocalIncludeIssues(const SourceSnapshot &snapshot, const RepoUniverse *universe)
{
  std::vector<ProjectFile> files;
  std::vector<const std::string *> contents;
  for (const auto &sf : snapshot.files())
  {
    if (!sf.authored)
      continue;
    files.push_back(ProjectFile{sf.path});
    contents.push_back(&sf.content);
  }
  const ProjectIndex index = buildProjectIndex(files);
  const CiSuffixIndex ciSuffix = build_ci_suffix_index(files);

  ScanContext ctx{files, index, ciSuffix, universe, {}, {}};
  if (universe != nullptr)
  {
    ctx.universeIndex = buildProjectIndex(universe->files);
    ctx.universeCi = build_ci_suffix_index(universe->files);
  }

  std::vector<LocalIncludeIssue> issues;
  for (std::size_t i = 0; i < files.size(); ++i)
    for (const IncludeDirective &d : scanIncludes(*contents[i]).directives)
      if (auto issue = classify(d, files[i].path, ctx))
        issues.push_back(std::move(*issue));
  return issues;
}

} // namespace archcheck::scan
