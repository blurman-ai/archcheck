#pragma once

#include <string>
#include <vector>

#include "archcheck/scan/project_files.h"

namespace archcheck::scan
{

// Abstraction over "list project files + read their content".
// Lets the graph builder plug in a git-blob source for `--diff` instead
// of paying the cost of materialising a worktree on disk (#024).
class FileSource
{
public:
  virtual ~FileSource() = default;

  // Enumerate project files. Paths are repo-relative, POSIX separators,
  // sorted for determinism. NodeId of a file equals its index in the vector.
  virtual std::vector<ProjectFile> list() = 0;

  // Read the content of a single file previously returned by list().
  virtual std::string read(const std::string &repoRelativePath) = 0;

protected:
  FileSource() = default;
  FileSource(const FileSource &) = default;
  FileSource &operator=(const FileSource &) = default;
  FileSource(FileSource &&) = default;
  FileSource &operator=(FileSource &&) = default;
};

} // namespace archcheck::scan
