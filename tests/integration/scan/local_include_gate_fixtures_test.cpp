#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <string_view>

#include "archcheck/scan/disk_file_source.h"
#include "archcheck/scan/local_include_scan.h"
#include "archcheck/scan/source_snapshot.h"

using archcheck::scan::DiskFileSource;
using archcheck::scan::LocalIncludeIssue;
using archcheck::scan::RepoUniverse;
using archcheck::scan::scanLocalIncludeIssues;
using archcheck::scan::SourceSnapshot;

namespace
{

std::filesystem::path fixture(std::string_view name)
{
  return std::filesystem::path{ARCHCHECK_FIXTURES_DIR} / "local_include_gate" / name;
}

std::vector<LocalIncludeIssue> scan(std::string_view name)
{
  DiskFileSource src(fixture(name));
  const auto snapshot = SourceSnapshot::read(src);
  return scanLocalIncludeIssues(snapshot);
}

std::size_t countKind(const std::vector<LocalIncludeIssue> &issues, LocalIncludeIssue::Kind kind)
{
  std::size_t n = 0;
  for (const auto &i : issues)
    if (i.kind == kind)
      ++n;
  return n;
}

} // namespace

TEST_CASE("local_include_gate: fixture A exact + fixture D external → no issues", "[scan][local-include]")
{
  const auto issues = scan("pass");
  CHECK(issues.empty());
}

TEST_CASE("local_include_gate: fixture B case mismatch is reported with the resolved path", "[scan][local-include]")
{
  const auto issues = scan("fail_case_mismatch");
  REQUIRE(countKind(issues, LocalIncludeIssue::Kind::CaseMismatch) == 1);
  CHECK(countKind(issues, LocalIncludeIssue::Kind::Unresolved) == 0);

  const auto &m = issues.front();
  CHECK(m.include == "core/Board.hpp");
  CHECK(m.resolvedCaseInsensitive == "include/core/board.hpp");
  CHECK(m.file == "src/main.cpp");
  CHECK(m.line == 4);
}

TEST_CASE("local_include_gate: fixture C generated-style local include is unresolved, not case mismatch",
          "[scan][local-include]")
{
  const auto issues = scan("fail_unresolved");
  REQUIRE(countKind(issues, LocalIncludeIssue::Kind::Unresolved) == 1);
  CHECK(countKind(issues, LocalIncludeIssue::Kind::CaseMismatch) == 0);
  CHECK(issues.front().include == "generated/config.hpp");
  CHECK(issues.front().resolvedCaseInsensitive.empty());
}

namespace
{

// Scan `sub` inside the fixture as a subtree, with the fixture root as the
// enclosing-repo universe (what check_command does when root is inside a repo).
std::vector<LocalIncludeIssue> scanSubtree(std::string_view name, std::string_view sub)
{
  DiskFileSource src(fixture(name) / sub);
  const auto snapshot = SourceSnapshot::read(src);
  const RepoUniverse universe{std::string{sub}, archcheck::scan::discoverFiles(fixture(name))};
  return scanLocalIncludeIssues(snapshot, &universe);
}

} // namespace

TEST_CASE("local_include_gate: subtree scan is silent when the header exists in the enclosing repo",
          "[scan][local-include]")
{
  // Without the universe the include is Unresolved (include/ is outside the scan root)...
  DiskFileSource src(fixture("subtree") / "src");
  const auto snapshot = SourceSnapshot::read(src);
  REQUIRE(scanLocalIncludeIssues(snapshot).size() == 1);

  // ...with it, the exact-case file exists in the repo, so nothing is reported.
  CHECK(scanSubtree("subtree", "src").empty());
}

TEST_CASE("local_include_gate: subtree scan still reports a cross-boundary case mismatch", "[scan][local-include]")
{
  const auto issues = scanSubtree("fail_case_mismatch", "src");
  REQUIRE(countKind(issues, LocalIncludeIssue::Kind::CaseMismatch) == 1);
  CHECK(issues.front().resolvedCaseInsensitive == "include/core/board.hpp");
}

TEST_CASE("local_include_gate: subtree scan keeps a nowhere-existing include unresolved", "[scan][local-include]")
{
  const auto issues = scanSubtree("fail_unresolved", "src");
  REQUIRE(countKind(issues, LocalIncludeIssue::Kind::Unresolved) == 1);
}

TEST_CASE("local_include_gate: a self-include is not a phantom case mismatch against itself", "[scan][local-include]")
{
  // libjxl enc_fast_lossless.cc pattern: #include "self" for macro re-expansion.
  // The resolver downgrades the self-edge to Unresolved, but the exact-case file
  // exists, so nothing must be reported (regression guard for the #168 FP).
  CHECK(scan("pass_self_include").empty());
}
