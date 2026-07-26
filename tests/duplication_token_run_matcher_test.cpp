#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <utility>
#include <vector>

#include "archcheck/scan/duplication/token_run_matcher.h"

using namespace archcheck::scan::duplication;

namespace
{
// A ~40-token function body, and a Type-2 copy with every local name renamed. Type-2 is
// rename-blind at the normalized-token level, so the two align as one maximal run.
const std::string kBase = "int accumulate(int count)\n"
                          "{\n"
                          "  int alpha = count + 1;\n"
                          "  int beta = alpha * 2;\n"
                          "  int gamma = beta - alpha;\n"
                          "  int delta = gamma + beta;\n"
                          "  int epsilon = delta * gamma;\n"
                          "  int zeta = epsilon + delta;\n"
                          "  return zeta + alpha;\n"
                          "}\n";
const std::string kRenamed = "int accumulate(int count)\n"
                             "{\n"
                             "  int aa = count + 1;\n"
                             "  int bb = aa * 2;\n"
                             "  int cc = bb - aa;\n"
                             "  int dd = cc + bb;\n"
                             "  int ee = dd * cc;\n"
                             "  int ff = ee + dd;\n"
                             "  return ff + aa;\n"
                             "}\n";

std::vector<Pair> run(const std::vector<std::pair<std::string, std::string>> &files, const RunMatchOptions &opts)
{
  std::vector<Fragment> frags;
  std::vector<Pair> pairs;
  matchTokenRuns(files, opts, frags, pairs);
  return pairs;
}

// A control-flow-bearing block, copied verbatim three times in one file with a distinct
// function name each (the only difference). Bodies are identical, so the three form a
// self-similar clique.
const std::string kRepeatedTriple = "int alpha(int x)\n"
                                    "{\n"
                                    "  int s = 0;\n"
                                    "  for (int i = 0; i < x; ++i)\n"
                                    "  {\n"
                                    "    if (i % 2 == 0)\n"
                                    "      s += i;\n"
                                    "    else\n"
                                    "      s -= i;\n"
                                    "  }\n"
                                    "  return s;\n"
                                    "}\n"
                                    "\n"
                                    "int beta(int x)\n"
                                    "{\n"
                                    "  int s = 0;\n"
                                    "  for (int i = 0; i < x; ++i)\n"
                                    "  {\n"
                                    "    if (i % 2 == 0)\n"
                                    "      s += i;\n"
                                    "    else\n"
                                    "      s -= i;\n"
                                    "  }\n"
                                    "  return s;\n"
                                    "}\n"
                                    "\n"
                                    "int gamma(int x)\n"
                                    "{\n"
                                    "  int s = 0;\n"
                                    "  for (int i = 0; i < x; ++i)\n"
                                    "  {\n"
                                    "    if (i % 2 == 0)\n"
                                    "      s += i;\n"
                                    "    else\n"
                                    "      s -= i;\n"
                                    "  }\n"
                                    "  return s;\n"
                                    "}\n";
} // namespace

// The core edge (ADR-004): a renamed copy across two files aligns as one maximal run,
// window-independently. Duplo/PMD (line/id-sensitive) miss this class; the window core
// needs the whole body to fit one window. Here it is a single native run.
TEST_CASE("Window-free core: cross-file Type-2 renamed clone is one maximal run", "[duplication][window-free]")
{
  const auto pairs = run({{"a.cpp", kBase}, {"b.cpp", kRenamed}}, RunMatchOptions{});
  REQUIRE(pairs.size() == 1);
  REQUIRE(pairs.front().type == "RENAMED");
}

// THE PRODUCT'S CORE CAPABILITY (SSOT duplication_architecture.md §2: the sole detector is
// Type-3 / diverged "edited copies"). A block copied then edited in the middle — one operator
// flipped — must be reported as ONE diverged clone, not lost and not split. The edit breaks
// the identical token run in two, so only STITCHING across it yields a single span: without
// stitching this fixture produces two separate half-clones, hence `size()==1` fails; without
// diverged classification the type would be EXACT, hence `type=="MIXED"` fails. Expectations
// are derived from what a diverged copy IS, not from the implementation's output.
TEST_CASE("Window-free core: a DIVERGED copy (operator edited mid-body) is one MIXED clone",
          "[duplication][window-free]")
{
  std::string edited = kBase; // copy, then a single logic edit in the middle
  const std::string from = "gamma + beta", to = "gamma - beta";
  const auto pos = edited.find(from);
  REQUIRE(pos != std::string::npos); // guards the fixture itself against drift
  edited.replace(pos, from.size(), to);

  std::vector<Fragment> frags;
  std::vector<Pair> pairs;
  RunMatchOptions opts;
  opts.minTokens = 20; // each identical half exceeds this, so un-stitched would be TWO clones
  matchTokenRuns({{"orig.cpp", kBase}, {"copy.cpp", edited}}, opts, frags, pairs);

  REQUIRE(pairs.size() == 1);             // the two halves are ONE diverged clone, not two
  REQUIRE(pairs.front().type == "MIXED"); // the edit is captured, not smoothed to EXACT
  const Fragment &a = frags[pairs.front().a];
  REQUIRE(a.startLine <= 3); // span reaches above the line-6 edit...
  REQUIRE(a.endLine >= 8);   // ...and below it: the whole copied body, stitched across the edit
}

// The structural idiom guard (ADR-004 §Implementation): removing the fragmenter's implicit
// function-scope surfaces non-executable clones — namespace-preamble declaration runs, data
// tables, interface signatures. requireControlFlow drops a span with no if/for/while/return,
// while a real copied body (which branches or returns) survives. Verified: a duplicated
// constexpr declaration block is dropped; the same-shape body with control flow is kept.
TEST_CASE("Window-free core: control-flow guard drops non-executable idiom clones", "[duplication][window-free]")
{
  const std::string decls = "namespace one\n{\nconstexpr int alpha = 100;\nconstexpr int beta = 200;\n"
                            "constexpr int gamma = 300;\nconstexpr int delta = 400;\nconstexpr int epsilon = 500;\n}\n";
  const std::string declsCopy = "namespace two\n{\nconstexpr int aa = 100;\nconstexpr int bb = 200;\n"
                                "constexpr int cc = 300;\nconstexpr int dd = 400;\nconstexpr int ee = 500;\n}\n";
  RunMatchOptions guard;
  guard.minTokens = 20;
  guard.requireControlFlow = true;
  RunMatchOptions raw;
  raw.minTokens = 20;

  const auto files = std::vector<std::pair<std::string, std::string>>{{"a.cpp", decls}, {"b.cpp", declsCopy}};
  REQUIRE(!run(files, raw).empty());  // the declaration run IS matched as a maximal clone...
  REQUIRE(run(files, guard).empty()); // ...but has no control flow → dropped as idiom
  // A real body (kBase/kRenamed have a for-free but returning body) survives the guard.
  RunMatchOptions bodyGuard;
  bodyGuard.requireControlFlow = true;
  REQUIRE(run({{"a.cpp", kBase}, {"b.cpp", kRenamed}}, bodyGuard).size() == 1);
}

// The property the window model violates and this core is built to guarantee: raising the
// minimum size yields a STRICT SUBSET of pairs (monotonic), because min-size is a pure
// post-filter over a threshold-independent set of maximal runs (#200 becomes honest).
TEST_CASE("Window-free core: min-size is monotonic (raise N -> strict subset)", "[duplication][window-free]")
{
  const std::vector<std::pair<std::string, std::string>> files = {{"a.cpp", kBase}, {"b.cpp", kRenamed}};
  auto keys = [](const std::vector<Pair> &ps)
  {
    std::vector<std::pair<std::size_t, std::size_t>> k;
    for (const auto &p : ps)
      k.emplace_back(p.a, p.b);
    std::sort(k.begin(), k.end());
    return k;
  };

  // Property test, no magic counts: sweep ascending thresholds; each result must be a subset
  // of the one below it. Derived from what "minimum size" must mean, not from measured output.
  std::vector<std::size_t> thresholds = {10, 25, 40, 55, 70, 100};
  std::vector<std::vector<std::pair<std::size_t, std::size_t>>> sets;
  for (const auto t : thresholds)
  {
    RunMatchOptions o;
    o.minTokens = t;
    sets.push_back(keys(run(files, o)));
  }
  for (std::size_t i = 1; i < sets.size(); ++i)
    REQUIRE(std::includes(sets[i - 1].begin(), sets[i - 1].end(), sets[i].begin(), sets[i].end()));
  // Non-vacuous: the sweep actually shrinks (a low threshold finds the clone, a high one drops
  // it) — otherwise "subset" would hold trivially over empty sets.
  REQUIRE(!sets.front().empty());
  REQUIRE(sets.back().size() < sets.front().size());
}

// #201 acceptance: a block copied 3x in one file (a self-similar clique) must have EVERY
// instance covered by at least one reported pair — the recall contract the dedup/tandem/stitch
// path must not violate — while the pair count stays bounded (a clique of k copies yields at
// most k*(k-1)/2 pairs; it must not explode). Guards the O(n^2) diagonal-indexed dedup/stitch
// rework against over-collapsing repeated regions.
TEST_CASE("Window-free core: a 3x-repeated block covers every instance, bounded pairs (#201)",
          "[duplication][window-free]")
{
  std::vector<Fragment> frags;
  std::vector<Pair> pairs;
  RunMatchOptions o;
  o.minLines = 6;
  matchTokenRuns({{"triple.cpp", kRepeatedTriple}}, o, frags, pairs);

  // The three instances occupy these body line ranges (alpha/beta/gamma; see kRepeatedTriple).
  const std::vector<std::pair<int, int>> instances = {{1, 12}, {14, 25}, {27, 38}};
  const auto covered = [&](std::pair<int, int> inst)
  {
    for (const auto &f : frags)
      if (f.startLine <= inst.second && inst.first <= f.endLine) // overlaps the instance
        return true;
    return false;
  };
  for (const auto &inst : instances)
    REQUIRE(covered(inst)); // recall contract: no instance is dropped by dedup/tandem collapse

  REQUIRE(pairs.size() >= 2); // >=2 pairs are needed to touch all three instances
  REQUIRE(pairs.size() <= 3); // bounded: a 3-clique is at most 3 pairs, not an explosion
  for (const auto &p : pairs)
    REQUIRE(p.type == "EXACT"); // identical bodies -> exact clones
}
