#include <catch2/catch_test_macros.hpp>

#include "archcheck/scan/duplication/fragmenter.h"
#include "archcheck/scan/duplication/token_normalizer.h"

using namespace archcheck::scan::duplication;

namespace
{

std::vector<Fragment> extractSmallFragments(const std::string &src)
{
  FragmentOptions opts;
  opts.minTokens = 5;
  return extractFragments(lex(src), src, "switch.cpp", opts);
}

} // namespace

TEST_CASE("Fragmenter: empty source", "[duplication]")
{
  const std::string src;
  const auto tokens = lex(src);
  const auto frags = extractFragments(tokens, src, "test.cpp");

  REQUIRE(frags.empty());
}

TEST_CASE("Fragmenter: no functions", "[duplication]")
{
  const std::string src = "int x = 5; int y = 10;";
  const auto tokens = lex(src);
  const auto frags = extractFragments(tokens, src, "test.cpp");

  REQUIRE(frags.empty());
}

TEST_CASE("Fragmenter: simple function", "[duplication]")
{
  const std::string src = "void f() { a=1; b=2; c=3; d=4; e=5; f=6; g=7; h=8; }";
  const auto tokens = lex(src);
  const auto frags = extractFragments(tokens, src, "test.cpp");

  REQUIRE(frags.size() >= 1);
  if (!frags.empty())
  {
    REQUIRE(frags[0].file == "test.cpp");
    REQUIRE(frags[0].startLine >= 1);
    REQUIRE(frags[0].endLine >= frags[0].startLine);
  }
}

TEST_CASE("Fragmenter: fragment size limits", "[duplication]")
{
  FragmentOptions opts;
  opts.minTokens = 5;
  opts.maxTokens = 100;

  const std::string src = "void f() { a; }"; // < 5 tokens inside braces
  const auto tokens = lex(src);
  const auto frags = extractFragments(tokens, src, "test.cpp", opts);

  REQUIRE(frags.empty()); // too small
}

TEST_CASE("Fragmenter: deeply nested blocks do not overflow the stack", "[duplication]")
{
  // ctags vendored an LLVM parser_overflow fixture with ~16k nested braces; the
  // old recursive collect() blew the call stack (SIGSEGV under -O2). The iterative
  // walker must handle arbitrary nesting. 100k levels overflows the old recursion
  // even in a debug build, so completing at all is the regression assertion.
  const std::size_t depth = 100000;
  std::string src;
  src.reserve(depth * 2);
  src.append(depth, '{');
  src.append(depth, '}');

  const auto tokens = lex(src);
  const auto frags = extractFragments(tokens, src, "deep.cpp");

  REQUIRE(frags.empty()); // bare nested blocks: no function bodies to fragment
}

TEST_CASE("Fragmenter: trigram diversity calculated", "[duplication]")
{
  const std::string src = "void f() { a = b + c; d = e * f; g = h - i; }";

  const auto tokens = lex(src);
  const auto frags = extractFragments(tokens, src, "diverse.cpp");

  if (!frags.empty())
  {
    // Diversity should be calculated
    REQUIRE(frags[0].diversity >= 0.0);
    REQUIRE(frags[0].diversity <= 1.0);
  }
}

TEST_CASE("Fragmenter: switch skeleton lines are not substantive line signal", "[duplication]")
{
  const std::string src = R"(
int dispatch(int op)
{
  switch (op)
  {
  case 1:
    return compute_one(op);
  case 2:
    return compute_two(op);
  default:
    break;
  }
  return fallback(op);
}
)";

  const auto frags = extractSmallFragments(src);

  // Two fragments since #190: the function body, then the nested switch block.
  // frags[0] is the outer body (the fragmenter emits enclosing-first).
  REQUIRE(frags.size() == 2);
  REQUIRE(frags[0].normLines.count("switch (op)") == 0);
  REQUIRE(frags[0].normLines.count("case 1:") == 0);
  REQUIRE(frags[0].normLines.count("case 2:") == 0);
  REQUIRE(frags[0].normLines.count("default:") == 0);
  REQUIRE(frags[0].normLines.count("break;") == 0);
  REQUIRE(frags[0].normLines.count("return compute_one(op);") == 1);
  REQUIRE(frags[0].normLines.count("return fallback(op);") == 1);
}

TEST_CASE("Fragmenter: inline switch table rows are not substantive line signal", "[duplication]")
{
  const std::string src = R"(
int flagsFor(int op)
{
  int flags = 0;
  switch (op)
  {
  case 0: flags |= Flag0; break;
  case 1: flags |= Flag1; break;
  default: flags |= DefaultFlag; break;
  }
  return flags;
}
)";

  const auto frags = extractSmallFragments(src);

  // Two fragments since #190: the function body, then the nested switch block.
  REQUIRE(frags.size() == 2);
  REQUIRE(frags[0].normLines.count("case 0: flags |= Flag0; break;") == 0);
  REQUIRE(frags[0].normLines.count("default: flags |= DefaultFlag; break;") == 0);
  REQUIRE(frags[0].normLines.count("int flags = 0;") == 1);
  REQUIRE(frags[0].normLines.count("return flags;") == 1);
}
