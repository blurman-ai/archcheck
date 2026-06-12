#pragma once

// Runs the real archcheck binary (ARCHCHECK_BINARY_PATH compile definition,
// see tests/CMakeLists.txt) and captures exit code + combined output. Shared
// by the product-level E2E tests.

#include <sys/wait.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "support/git_test_repo.h"

namespace archcheck::testsupport
{

struct RunResult
{
  int exitCode = -1;
  std::string output;
};

inline RunResult runArchcheck(const std::filesystem::path &cwd, const std::string &args)
{
  TempDir outDir;
  const auto outFile = outDir.path / "out.txt";
  const std::string cmd =
      "cd '" + cwd.string() + "' && '" ARCHCHECK_BINARY_PATH "' " + args + " > '" + outFile.string() + "' 2>&1";
  const int status = std::system(cmd.c_str());
  RunResult r;
  r.exitCode = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
  std::ifstream f(outFile);
  std::stringstream ss;
  ss << f.rdbuf();
  r.output = ss.str();
  return r;
}

} // namespace archcheck::testsupport
