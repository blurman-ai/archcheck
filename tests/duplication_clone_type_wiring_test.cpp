#include <catch2/catch_test_macros.hpp>

#include "archcheck/scan/duplication/duplication_scanner.h"

using namespace archcheck::scan::duplication;

namespace
{
// Plain-metric gate, no joint floor / P1 / whole-file guard: lets a single
// cross-file clone pair survive so we can assert the wired-in `type` field.
ScannerOptions wiringOpts()
{
  ScannerOptions opts;
  opts.metric = "plain";
  opts.simThreshold = 0.60;
  opts.enableJointFloor = false;
  opts.enableP1Guards = false;
  opts.enableWholeFileGuard = false;
  return opts;
}

const std::string kBody = R"(
void accumulate(const std::vector<int>& data) {
    int total = 0;
    for (int i = 0; i < data.size(); i++) {
        int value = data[i];
        if (value > limit) {
            total += value;
            seen.push_back(value);
        }
    }
    report(total);
}
)";
} // namespace

TEST_CASE("scanForDuplication tags identical cross-file clone as EXACT", "[duplication][clone-type][wiring]")
{
  std::vector<std::pair<std::string, std::string>> files = {{"a.cpp", kBody}, {"b.cpp", kBody}};

  const ScanResult result = scanForDuplication(files, wiringOpts());

  REQUIRE(result.pairs.size() == 1);
  REQUIRE(result.pairs[0].type == "EXACT");
}

TEST_CASE("scanForDuplication tags renamed-local clone as RENAMED", "[duplication][clone-type][wiring]")
{
  // b.cpp renames the local `value` -> `elem`; callee names and structure intact,
  // so the normalized stream matches and only identifier spellings differ.
  std::string renamed = kBody;
  for (std::string::size_type pos = renamed.find("value"); pos != std::string::npos; pos = renamed.find("value", pos))
  {
    renamed.replace(pos, 5, "elem0");
  }
  std::vector<std::pair<std::string, std::string>> files = {{"a.cpp", kBody}, {"b.cpp", renamed}};

  const ScanResult result = scanForDuplication(files, wiringOpts());

  REQUIRE(result.pairs.size() == 1);
  REQUIRE(result.pairs[0].type == "RENAMED");
}
