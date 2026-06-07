#include <catch2/catch_test_macros.hpp>
#include <string>

#include "archcheck/scan/duplication/fragmenter.h"
#include "archcheck/scan/duplication/token_normalizer.h"

using namespace archcheck::scan::duplication;

// #091 regression. Real case: LibreSprite src/doc/algo.cpp has near-identical
// twin functions (algo_line / algo_line_float) that NiCad flags as a clone but
// archcheck missed. Root cause: algo_line was 392 tokens (kept whole) while its
// float twin exceeded the old 400-token cap and was subdivided, so the two never
// produced comparable fragments. A flat function body over the cap with no inner
// braces cannot be subdivided at all — it drops to zero fragments and vanishes
// from the clone search. maxTokens=600 keeps such functions whole.

// Builds a flat (no nested braces) function body of a controllable token size.
static std::string bigFunction(const std::string &name, const std::string &v, int iters)
{
  std::string s = "int " + name + "(int " + v + "0, int " + v + "1, int* out)\n{\n";
  s += "  int " + v + "a = " + v + "0;\n  int " + v + "b = " + v + "1;\n  int acc = 0;\n";
  for (int i = 0; i < iters; ++i)
  {
    s += "  " + v + "a = " + v + "a * 3 + " + std::to_string(i) + ";\n";
    s += "  " + v + "b = " + v + "b - " + v + "a + " + std::to_string(i * 2) + ";\n";
    s += "  acc = acc + " + v + "a * " + v + "b - " + std::to_string(i) + ";\n";
    s += "  out[" + std::to_string(i) + "] = acc + " + v + "a;\n";
  }
  s += "  return acc;\n}\n";
  return s;
}

TEST_CASE("fragmenter keeps a >400-token function whole at the default cap (#091)", "[duplication][fragmenter]")
{
  const std::string src = bigFunction("compute", "v", 12); // ~450 tokens
  const auto tokens = lex(src, true);
  REQUIRE(tokens.size() > 400);  // exceeds the old cap
  REQUIRE(tokens.size() <= 600); // within the new cap

  // Old cap (400): a flat body over the cap has no inner braces to descend into,
  // so it produces zero fragments and disappears from the clone search.
  FragmentOptions oldCap;
  oldCap.maxTokens = 400;
  REQUIRE(extractFragments(tokens, src, "old.cpp", oldCap).empty());

  // Default cap (600): the whole function survives as a single fragment, so a
  // near-identical twin of the same size will align with it.
  const auto whole = extractFragments(tokens, src, "new.cpp");
  REQUIRE(whole.size() == 1);
  REQUIRE(whole.front().tokenCount > 400);
}
