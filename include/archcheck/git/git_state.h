#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace archcheck::git
{

struct GitError
{
  std::string message;
};

// Parsed git revspec passed to --diff.
//
//   "a..b"   → {a, b}            (two explicit refs)
//   "<ref>"  → {<ref>, "HEAD"}   (implicit "vs working tree HEAD")
struct Revspec
{
  std::string baseline;
  std::string current;
};

std::optional<Revspec> parseRevspec(std::string_view input);

// Locate the enclosing git repo root for `inside` (equivalent to
// `git rev-parse --show-toplevel`). Empty on failure.
std::optional<std::filesystem::path> findRepoRoot(const std::filesystem::path &inside);

// RAII handle for a temporary `git worktree add --detach <tmp> <ref>`.
// Destruction runs `git worktree remove --force <tmp>` (best-effort) and,
// as a fallback, deletes the directory. Move-only.
class Worktree
{
public:
  Worktree() = default;
  Worktree(std::filesystem::path repoRoot, std::filesystem::path workPath);
  Worktree(const Worktree &) = delete;
  Worktree &operator=(const Worktree &) = delete;
  Worktree(Worktree &&other) noexcept;
  Worktree &operator=(Worktree &&other) noexcept;
  ~Worktree();

  const std::filesystem::path &path() const { return work_; }

private:
  void release();
  std::filesystem::path repo_;
  std::filesystem::path work_;
};

// Materialise `ref` (any git revspec) into a temporary worktree under the
// system temp directory. The returned Worktree owns cleanup. Special-case:
// when ref == "WORKTREE", returns a non-owning Worktree pointing at
// `repoRoot` itself (so callers can use the working tree without a checkout).
std::optional<Worktree> materializeRef(const std::filesystem::path &repoRoot, const std::string &ref, GitError &err);

// Sentinel revspec component that means "current working tree on disk".
inline constexpr const char *kWorktreeRef = "WORKTREE";

// List repo-relative paths that look like C/C++ source AND changed between
// `baselineRef` and `currentRef`. `currentRef == kWorktreeRef` means "the
// working tree on disk" — both tracked-modified and untracked files are
// included. Used by `--diff` as a fast-path: if the list is empty, the graph
// cannot have changed, so we skip the (expensive) worktree materialisation
// and graph rebuild entirely.
//
// On git failure returns std::nullopt (caller falls back to the full path).
// An empty vector is a valid result (== "no relevant changes").
std::optional<std::vector<std::filesystem::path>>
changedCppFiles(const std::filesystem::path &repoRoot, const std::string &baselineRef, const std::string &currentRef);

} // namespace archcheck::git
