#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "archcheck/scan/authored_scope.h"

#include "support/run_archcheck.h"

using archcheck::scan::AuthoredScope;
using archcheck::testsupport::runArchcheck;

namespace
{

std::filesystem::path fixtureRoot(const std::string &name)
{
  return std::filesystem::path{ARCHCHECK_FIXTURES_DIR} / "vendor_exclusion" / name;
}

std::string readFile(const std::filesystem::path &path)
{
  std::ifstream f(path, std::ios::binary);
  REQUIRE(f.is_open());
  return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

// Walks a fixture tree and collects every regular file as (repo-relative
// POSIX path, content) — the shape AuthoredScope::fromFiles expects.
std::vector<std::pair<std::string, std::string>> collectFiles(const std::filesystem::path &root)
{
  std::vector<std::pair<std::string, std::string>> files;
  for (const auto &entry : std::filesystem::recursive_directory_iterator(root))
  {
    if (!entry.is_regular_file())
    {
      continue;
    }
    files.push_back({std::filesystem::relative(entry.path(), root).generic_string(), readFile(entry.path())});
  }
  return files;
}

bool hasSuffix(const std::string &s, std::string_view suffix)
{
  return s.size() >= suffix.size() && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool isOwnSource(const std::string &path)
{
  return path.rfind("own/", 0) == 0 && (hasSuffix(path, ".h") || hasSuffix(path, ".cpp"));
}

} // namespace

TEST_CASE("vendor_exclusion fixtures: nested-directory license banner excludes only the vendored subtree",
          "[scan][vendor_exclusion][fixtures]")
{
  const auto files = collectFiles(fixtureRoot("pass_nested_license"));
  const auto scope = AuthoredScope::fromFiles(files);

  bool sawVendored = false;
  bool sawOwn = false;
  for (const auto &[path, content] : files)
  {
    if (path == "libs/foo/render.cpp")
    {
      REQUIRE(scope.excluded(path, content));
      sawVendored = true;
    }
    else if (isOwnSource(path))
    {
      REQUIRE_FALSE(scope.excluded(path, content));
      sawOwn = true;
    }
  }
  REQUIRE(sawVendored);
  REQUIRE(sawOwn);
}

TEST_CASE("vendor_exclusion fixtures: generated @-banner excludes regardless of banner ratio",
          "[scan][vendor_exclusion][fixtures]")
{
  const auto files = collectFiles(fixtureRoot("pass_generated_marker"));
  REQUIRE(files.size() == 1);
  const auto scope = AuthoredScope::fromFiles(files);
  REQUIRE(scope.excluded(files[0].first, files[0].second));
}

TEST_CASE("vendor_exclusion fixtures: minified data blob is excluded", "[scan][vendor_exclusion][fixtures]")
{
  const auto files = collectFiles(fixtureRoot("pass_minified_data"));
  REQUIRE(files.size() == 1);
  const auto scope = AuthoredScope::fromFiles(files);
  REQUIRE(scope.excluded(files[0].first, files[0].second));
}

TEST_CASE("vendor_exclusion fixtures: own code with red-herring prose is NOT excluded (anti-over-exclude)",
          "[scan][vendor_exclusion][fixtures]")
{
  const auto root = fixtureRoot("fail_own_code");
  const auto files = collectFiles(root);
  REQUIRE(files.size() == 1);
  const auto scope = AuthoredScope::fromFiles(files);
  REQUIRE_FALSE(scope.excluded(files[0].first, files[0].second));

  // The real SF.7 violation must reach the report, not just survive the
  // AuthoredScope-level check (#153: "the exact prose strings that
  // previously ate own code").
  const auto r = runArchcheck(root, "--format json .");
  REQUIRE(r.output.find("\"rule\": \"SF.7\"") != std::string::npos);
}
