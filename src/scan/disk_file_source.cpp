#include "archcheck/scan/disk_file_source.h"

#include <fstream>
#include <iterator>
#include <utility>

namespace archcheck::scan
{

DiskFileSource::DiskFileSource(std::filesystem::path root) : root_(std::move(root)) {}

std::vector<ProjectFile> DiskFileSource::list() { return discoverFiles(root_); }

std::string DiskFileSource::read(const std::string &repoRelativePath)
{
  std::ifstream f(root_ / repoRelativePath, std::ios::binary);
  return std::string{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

} // namespace archcheck::scan
