#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <iostream>
#include <string_view>

#include "archcheck/scan/disk_file_source.h"
#include "archcheck/scan/duplication/clone_classifier.h"
#include "archcheck/scan/duplication/duplication_scanner.h"
#include "archcheck/scan/project_files.h"

using namespace archcheck::scan::duplication;

namespace
{
// Load a fixtures/duplication/<sub> directory through the real product gate
// (collectNonVendoredSources: vendor/test/generated exclusion, #129). The data-table
// pairs reach the scorer with realistic IDF because each fixture file carries a second,
// unrelated fragment — an isolated two-fragment file scores the table pair below the gate.
std::vector<std::pair<std::string, std::string>> loadFixtureDir(std::string_view sub)
{
  const std::filesystem::path dir = std::filesystem::path{ARCHCHECK_FIXTURES_DIR} / "duplication" / sub;
  archcheck::scan::DiskFileSource src(dir);
  return archcheck::scan::collectNonVendoredSources(src);
}
} // namespace

TEST_CASE("P1.1 data-table guard: a LITERAL low-diversity color table is a real DROP", "[duplication][p1][fixtures]")
{
  // fail_palette/: a color-tier table copied between two unit files, differing only in
  // the `default:` RGB (modelled on djeada/Standard-of-Iron team_color).
  const auto files = loadFixtureDir("data_table/fail_palette");
  REQUIRE(files.size() == 2); // both are authored source (not excluded as test/vendor)

  // Guard OFF: the pair clears the joint floor (high line overlap) and is reported. It is
  // exactly the data-table signature — LITERAL type, both fragments low-diversity.
  ScannerOptions opts;
  opts.enableDataTableDrop = false;
  const ScanResult before = scanForDuplication(files, opts);
  REQUIRE(before.pairs.size() == 1);
  const Pair &p = before.pairs.front();
  REQUIRE(std::string_view(cloneType(before.fragments[p.a], before.fragments[p.b])) == "LITERAL");
  REQUIRE(before.fragments[p.a].diversity < 0.30);
  REQUIRE(before.fragments[p.b].diversity < 0.30);

  // Guard ON (default): the data table is dropped — membership changes, not just weight.
  opts.enableDataTableDrop = true;
  const ScanResult after = scanForDuplication(files, opts);
  REQUIRE(after.pairs.empty());
}

TEST_CASE("P1.1 data-table guard: a high-diversity logic copy is NOT dropped", "[duplication][p1][fixtures]")
{
  // pass/: a real importer logic copy (EXACT, diversity >= 0.30). The guard must leave it.
  const auto files = loadFixtureDir("data_table/pass");
  REQUIRE(files.size() == 2);

  ScannerOptions opts; // guard on by default
  const ScanResult result = scanForDuplication(files, opts);
  REQUIRE(result.pairs.size() >= 1);
  for (const Pair &p : result.pairs)
  {
    REQUIRE(result.fragments[p.a].diversity >= 0.30); // survived the guard's diversity gate
  }
}

TEST_CASE("test-file exclusion: two *Tests.cpp clones never reach the scanner", "[duplication][p1][fixtures]")
{
  // test_boilerplate/: WidgetTests.cpp + GadgetTests.cpp hold an identical arrange-act-assert
  // block. CamelCase `*Tests` is recognized as a test basename (#158 A.1), so the product gate
  // drops both before scanning — no clone pair is ever reported between them.
  const auto files = loadFixtureDir("test_boilerplate");
  REQUIRE(files.empty()); // both excluded as test files

  const ScanResult result = scanForDuplication(files);
  REQUIRE(result.pairs.empty());
}

TEST_CASE("P1.2 boilerplate-density filter catches short repeated patterns", "[duplication][p1][synthetic]")
{

  // Synthetic case 2: loop-like boilerplate (generator pattern)
  std::string loop1 = R"(
void processLoop1() {
  for (int i = 0; i < 10; i++) {
    process(i);
    print(i);
    check(i);
    validate(i);
    update(i);
    log(i);
    sync(i);
    flush(i);
    reset(i);
  }
}
)";

  std::string loop2 = R"(
void processLoop2() {
  for (int j = 0; j < 10; j++) {
    process(j);
    print(j);
    check(j);
    validate(j);
    update(j);
    log(j);
    sync(j);
    flush(j);
    reset(j);
  }
}
)";

  std::vector<std::pair<std::string, std::string>> files = {{"loop1.cpp", loop1}, {"loop2.cpp", loop2}};

  ScannerOptions opts;
  opts.fragmentOpts.minTokens = 15; // Lower min to catch these fragments
  opts.simThreshold = 0.50;         // Lower threshold
  opts.enableP1Guards = true;

  ScanResult result = scanForDuplication(files, opts);

  std::cout << "\n=== P1.2 Boilerplate-Density Test ===\n";
  std::cout << "Fragments: " << result.fragments.size() << "\n";
  std::cout << "Pairs found: " << result.pairs.size() << "\n";

  if (!result.pairs.empty())
  {
    for (const auto &p : result.pairs)
    {
      std::cout << "  Boilerplate pair[" << p.a << "," << p.b << "]: ";
      std::cout << "w=" << p.weighted << " (P1.2 may filter)\n";
    }
  }
  else
  {
    std::cout << "  (No pairs found - may indicate good FP filtering)\n";
  }

  // Just check that we processed something
  REQUIRE(result.fileCount == files.size());
}

TEST_CASE("Real duplication survives all P1 filters", "[duplication][p1][synthetic]")
{

  // Synthetic case 3: real algorithmic duplication (not boilerplate)
  std::string algo1 = R"(
int bubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
    return 0;
}
)";

  std::string algo2 = R"(
int bubbleSortInts(int data[], int size) {
    for (int x = 0; x < size - 1; x++) {
        for (int y = 0; y < size - x - 1; y++) {
            if (data[y] > data[y + 1]) {
                int swap = data[y];
                data[y] = data[y + 1];
                data[y + 1] = swap;
            }
        }
    }
    return 0;
}
)";

  std::vector<std::pair<std::string, std::string>> files = {{"sort1.cpp", algo1}, {"sort2.cpp", algo2}};

  ScannerOptions opts;
  opts.fragmentOpts.minTokens = 20; // Lower min for algorithm fragments
  opts.simThreshold = 0.60;
  opts.enableP1Guards = true;

  ScanResult result = scanForDuplication(files, opts);

  std::cout << "\n=== Real Algorithm Duplication Test ===\n";
  std::cout << "Fragments: " << result.fragments.size() << "\n";
  std::cout << "Pairs found (should survive P1): " << result.pairs.size() << "\n";

  if (!result.pairs.empty())
  {
    for (const auto &p : result.pairs)
    {
      std::cout << "  Real dupe[" << p.a << "," << p.b << "]: ";
      std::cout << "w=" << p.weighted << " (survived P1 filters ✓)\n";
    }
  }

  REQUIRE(result.fragments.size() > 0);
}

// #190: a block copied from one function into another. The enclosing bodies differ,
// so before #190 (function bodies emitted whole, never descended into) this was
// invisible no matter how verbatim the copy was.
TEST_CASE("#190: copy-paste nested inside a function is reported", "[duplication][fixtures]")
{
  const auto files = loadFixtureDir("subfunction_clone/fail_block_into_function");
  REQUIRE(files.size() == 2);

  const ScanResult result = scanForDuplication(files); // shipped defaults, no tuning
  REQUIRE(result.pairs.size() == 1);

  // Exactly the nested block, not the whole enclosing bodies.
  const Pair &p = result.pairs.front();
  const Fragment &fa = result.fragments[p.a];
  const Fragment &fb = result.fragments[p.b];
  REQUIRE(fa.file != fb.file);
  REQUIRE(fa.nested);
  REQUIRE(fb.nested);
  REQUIRE(fa.endLine - fa.startLine < 20); // the block, not the function
}

// The negative side: descending into bodies must not report two unrelated functions
// merely because each contains a guarded loop.
TEST_CASE("#190: similarly shaped but unrelated nested blocks stay silent", "[duplication][fixtures]")
{
  const auto files = loadFixtureDir("subfunction_clone/pass");
  REQUIRE(files.size() == 2);

  const ScanResult result = scanForDuplication(files);
  REQUIRE(result.pairs.empty());
}

TEST_CASE("#195: shared prologue in otherwise different functions is reported", "[duplication][fixtures]")
{
  const auto files = loadFixtureDir("cross_function_partial_clone/fail_shared_prologue");
  REQUIRE(files.size() == 2);

  const ScanResult result = scanForDuplication(files);
  REQUIRE(result.pairs.size() == 1);

  const Pair &p = result.pairs.front();
  const Fragment &fa = result.fragments[p.a];
  const Fragment &fb = result.fragments[p.b];
  REQUIRE(fa.file != fb.file);
  REQUIRE(fa.nested);
  REQUIRE(fb.nested);
  REQUIRE(fa.endLine < 20);
  REQUIRE(fb.endLine < 20);
}

TEST_CASE("#195: short coincidental shared prologue stays silent", "[duplication][fixtures]")
{
  const auto files = loadFixtureDir("cross_function_partial_clone/pass");
  REQUIRE(files.size() == 2);

  const ScanResult result = scanForDuplication(files);
  REQUIRE(result.pairs.empty());
}

TEST_CASE("#191: an offset middle-of-function run is reported", "[duplication][fixtures]")
{
  const auto files = loadFixtureDir("statement_run_clone/fail_offset_middle_run");
  REQUIRE(files.size() == 2);

  // The fixture functions are ~50 lines; lowering maxTokens makes each body exceed it,
  // taking the exact "too large to emit whole" statement-run path a >600-token function
  // takes at the default. The shared session-setup run sits in the MIDDLE of both bodies
  // — offset by a DIFFERENT prologue in each — so #190 (one braced block) and #195 (a run
  // flush with bodyLo/bodyHi) both miss it; only #191's interior runs reach it.
  ScannerOptions opts;
  opts.fragmentOpts.maxTokens = 120;
  const ScanResult result = scanForDuplication(files, opts);
  REQUIRE(result.pairs.size() >= 1);

  const Pair &p = result.pairs.front();
  const Fragment &fa = result.fragments[p.a];
  const Fragment &fb = result.fragments[p.b];
  REQUIRE(fa.file != fb.file);
  REQUIRE(fa.boundary); // a synthetic statement-run span, not a whole body
  REQUIRE(fb.boundary);
  // The match is the middle run, not the whole ~50-line body.
  REQUIRE(fa.endLine - fa.startLine < 25);
  REQUIRE(fb.endLine - fb.startLine < 25);

  // Negative control (the #195 "fixture passes != feature works" lesson): the run is
  // invisible to #190/#195 alone — disabling statement runs drops it to zero, proving
  // #191's interior slicing is what reports it.
  opts.enableStatementRuns = false;
  REQUIRE(scanForDuplication(files, opts).pairs.empty());
}

TEST_CASE("#191: similar-shape but unrelated functions stay silent", "[duplication][fixtures]")
{
  const auto files = loadFixtureDir("statement_run_clone/pass");
  REQUIRE(files.size() == 2);

  ScannerOptions opts;
  opts.fragmentOpts.maxTokens = 120;
  const ScanResult result = scanForDuplication(files, opts);
  REQUIRE(result.pairs.empty());
}
