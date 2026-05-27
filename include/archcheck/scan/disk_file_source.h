#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "archcheck/scan/file_source.h"
#include "archcheck/scan/project_files.h"

namespace archcheck::scan
{

// FileSource backed by std::filesystem: enumerates files under `root`
// using discoverFiles() and reads content with std::ifstream.
class DiskFileSource final : public FileSource
{
public:
  explicit DiskFileSource(std::filesystem::path root);

  std::vector<ProjectFile> list() override;
  std::string read(const std::string &repoRelativePath) override;

  const std::filesystem::path &root() const { return root_; }

private:
  std::filesystem::path root_;
};

} // namespace archcheck::scan
