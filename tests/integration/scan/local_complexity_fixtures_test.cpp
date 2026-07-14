#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <string>

#include "archcheck/scan/local_complexity_absolute.h"
#include "archcheck/scan/local_complexity_drift.h"
#include "archcheck/scan/local_complexity_metrics.h"
#include "archcheck/scan/source_snapshot.h"

namespace
{

std::string fixture(const std::string &name)
{
  const auto path = std::filesystem::path{ARCHCHECK_FIXTURES_DIR} / "local_complexity_drift" / name;
  std::ifstream f(path);
  REQUIRE(f.is_open());
  return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

std::string absoluteFixture(const std::string &name)
{
  const auto path = std::filesystem::path{ARCHCHECK_FIXTURES_DIR} / "local_complexity_absolute" / name;
  std::ifstream f(path);
  REQUIRE(f.is_open());
  return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

struct MapFileSource final : archcheck::scan::FileSource
{
  std::map<std::string, std::string> files;

  std::vector<archcheck::scan::ProjectFile> list() override
  {
    std::vector<archcheck::scan::ProjectFile> out;
    for (const auto &[path, content] : files)
      out.push_back({path});
    return out;
  }
  std::string read(const std::string &path) override
  {
    const auto it = files.find(path);
    return it == files.end() ? std::string{} : it->second;
  }
};

// Score one absolute fixture file through a real SourceSnapshot at `threshold`.
archcheck::rules::ViolationList absoluteFindings(const std::string &name, std::size_t threshold)
{
  MapFileSource src;
  src.files["src/sample.cpp"] = absoluteFixture(name);
  const auto snapshot = archcheck::scan::SourceSnapshot::read(src);
  return archcheck::scan::detectAbsoluteComplexity(snapshot, threshold);
}

TEST_CASE("complexity fixtures: flat branches stay below any alarm", "[scan][complexity][fixtures]")
{
  const auto fns = archcheck::scan::computeFileComplexity(fixture("pass/flat_branches.cpp"));
  REQUIRE(fns.size() == 1);
  REQUIRE(fns[0].score == 3);
  REQUIRE(fns[0].maxNesting <= 1);
}

TEST_CASE("complexity fixtures: comments and strings carry no branch tokens", "[scan][complexity][fixtures]")
{
  const auto fns = archcheck::scan::computeFileComplexity(fixture("pass/comments_and_strings.cpp"));
  REQUIRE(fns.size() == 1);
  REQUIRE(fns[0].score == 0);
}

TEST_CASE("complexity fixtures: preprocessor branches score 0", "[scan][complexity][fixtures]")
{
  const auto fns = archcheck::scan::computeFileComplexity(fixture("pass/preprocessor_lines.cpp"));
  REQUIRE(fns.size() == 1);
  REQUIRE(fns[0].score == 0);
}

TEST_CASE("complexity fixtures: nested growth baseline -> current is a finding", "[scan][complexity][fixtures]")
{
  const auto res = archcheck::scan::compareLocalComplexity(fixture("fail_growth/update_baseline.cpp"),
                                                           fixture("fail_growth/update_current.cpp"), "update.cpp");
  REQUIRE(res.violations.size() == 1);
  REQUIRE(res.violations[0].ruleId == "DRIFT.LOCAL_COMPLEXITY");
  REQUIRE(res.violations[0].message.find("from 1 to 10") != std::string::npos);
}

TEST_CASE("complexity fixtures: harmless append is silent", "[scan][complexity][fixtures]")
{
  const auto res = archcheck::scan::compareLocalComplexity(fixture("pass/harmless_change_baseline.cpp"),
                                                           fixture("pass/harmless_change_current.cpp"), "act.cpp");
  REQUIRE(res.violations.empty());
  REQUIRE(res.positiveDelta == 0);
}

TEST_CASE("complexity fixtures: new function above 25 is reported", "[scan][complexity][fixtures]")
{
  const auto res =
      archcheck::scan::compareLocalComplexity("", fixture("fail_new_complex/new_complex_function.cpp"), "new.cpp");
  REQUIRE(res.violations.size() == 1);
  REQUIRE(res.violations[0].message.find("new function 'saturate'") != std::string::npos);
}

TEST_CASE("absolute complexity: flat function stays silent below the ceiling", "[scan][complexity][fixtures]")
{
  const auto flat = archcheck::scan::computeFileComplexity(absoluteFixture("pass/flat_three_ifs.cpp"));
  REQUIRE(flat.size() == 1);
  REQUIRE(flat[0].score == 3);
  // Silent even at the default ceiling (40).
  REQUIRE(absoluteFindings("pass/flat_three_ifs.cpp", 40).empty());
  REQUIRE(absoluteFindings("pass/early_returns.cpp", 40).empty());
}

TEST_CASE("absolute complexity: deep nesting trips FUNC.COGNITIVE_COMPLEXITY at the default",
          "[scan][complexity][fixtures]")
{
  const auto found = absoluteFindings("fail_nested/nested_if_loop.cpp", 40);
  REQUIRE(found.size() == 1);
  CHECK(found[0].ruleId == "FUNC.COGNITIVE_COMPLEXITY");
  CHECK(found[0].file == "src/sample.cpp");
  CHECK(found[0].message.find("'scan'") != std::string::npos);
}

TEST_CASE("absolute complexity: long boolean chain contributes but stays flat", "[scan][complexity][fixtures]")
{
  const auto fns = archcheck::scan::computeFileComplexity(absoluteFixture("pass/long_boolean_chain.cpp"));
  REQUIRE(fns.size() == 1);
  CHECK(fns[0].maxNesting <= 1);
  CHECK(fns[0].score > 3); // operator-alternation cost, no nesting
  // A wall of &&/|| costs something, but without nesting it stays well under the
  // default ceiling — only a deliberately low threshold surfaces it.
  CHECK(absoluteFindings("pass/long_boolean_chain.cpp", 40).empty());
  REQUIRE(absoluteFindings("pass/long_boolean_chain.cpp", 10).size() == 1);
}

} // namespace
