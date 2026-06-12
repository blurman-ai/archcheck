#pragma once

// Shared scaffolding for integration tests that synthesize small git repos:
// mkdtemp-backed temp dir, git init with deterministic config, commit-all.
// NB: git_diff_test.cpp / git_object_file_source_test.cpp predate this header
// and still carry local copies — migrating them is tracked in #108.

#include <unistd.h>

#include <catch2/catch_test_macros.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace archcheck::testsupport
{

struct TempDir
{
  std::filesystem::path path;
  TempDir()
  {
    const auto tmpl = (std::filesystem::temp_directory_path() / "archcheck-itest-XXXXXX").string();
    std::vector<char> buf(tmpl.begin(), tmpl.end());
    buf.push_back('\0');
    REQUIRE(mkdtemp(buf.data()) != nullptr);
    path = std::filesystem::path{buf.data()};
  }
  ~TempDir()
  {
    std::error_code ec;
    std::filesystem::remove_all(path, ec);
  }
  TempDir(const TempDir &) = delete;
  TempDir &operator=(const TempDir &) = delete;
};

// std::system is fine here — the only interpolated string is `cwd`, which
// comes from mkdtemp(3) (/tmp/archcheck-itest-XXXXXX, no shell metachars).
inline int runIn(const std::filesystem::path &cwd, const std::string &cmd)
{
  const std::string full = "cd '" + cwd.string() + "' && " + cmd + " >/dev/null 2>&1";
  return std::system(full.c_str());
}

inline void writeFile(const std::filesystem::path &p, std::string_view content)
{
  std::ofstream f(p);
  f << content;
}

inline void initRepo(const std::filesystem::path &p)
{
  REQUIRE(runIn(p, "git init -q -b main") == 0);
  REQUIRE(runIn(p, "git config user.email test@example") == 0);
  REQUIRE(runIn(p, "git config user.name test") == 0);
  REQUIRE(runIn(p, "git config commit.gpgsign false") == 0);
}

inline void commitAll(const std::filesystem::path &p, const std::string &msg)
{
  REQUIRE(runIn(p, "git add . && git commit -qm '" + msg + "'") == 0);
}

} // namespace archcheck::testsupport
