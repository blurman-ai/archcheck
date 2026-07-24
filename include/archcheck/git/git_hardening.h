#pragma once

#include <vector>

namespace archcheck::git
{

// S6 (#105): hardening flags prepended to every git invocation so an untrusted
// repo's .git/config cannot run commands via hooks / fsmonitor / pager. Single
// source of truth: runGit (one-shot, captures stdout+stderr) and
// GitObjectFileSource (long-running `cat-file --batch`, bidirectional stdio)
// each spawn git with their OWN pipe wiring, but MUST apply the same hardening
// policy — a divergence here is a security hole. Only the policy is shared; the
// fork/pipe plumbing stays separate because the two have different stdio
// topologies. diff.external is intentionally NOT suppressed (an empty value
// breaks `git diff` on git 2.30; callers pass --no-ext-diff instead).
inline constexpr const char *kGitHardeningArgs[] = {
    "-c", "core.hooksPath=/dev/null", "-c", "core.fsmonitor=", "-c", "core.pager=cat",
};
inline constexpr int kGitHardeningCount = 6; // entries in kGitHardeningArgs

// Build the argv prefix ["git", <hardening args>...] with the S6 policy applied.
// Each spawn path appends its own subcommand and a trailing nullptr before
// execvp; sharing this one assembly step (not just the array above) keeps the
// hardening application itself from drifting between the two fork sites.
// LCOV_EXCL_START — only ever called post-fork in the child, invisible to gcov.
inline std::vector<char *> hardenedGitArgv()
{
  std::vector<char *> argv;
  argv.reserve(kGitHardeningCount + 8);
  argv.push_back(const_cast<char *>("git"));
  for (int i = 0; i < kGitHardeningCount; ++i)
  {
    argv.push_back(const_cast<char *>(kGitHardeningArgs[i]));
  }
  return argv;
}
// LCOV_EXCL_STOP

} // namespace archcheck::git
