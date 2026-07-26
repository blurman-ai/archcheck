#include <algorithm>
#include <catch2/catch_test_macros.hpp>

#include "archcheck/scan/duplication/token_normalizer.h"

using namespace archcheck::scan::duplication;

TEST_CASE("Token normalizer: simple identifier", "[duplication]")
{
  const std::string src = "int x = 5;";
  const auto tokens = lex(src);

  REQUIRE(tokens.size() == 5);
  REQUIRE(tokens[0].sym == "int");
  REQUIRE(tokens[0].line == 1);
  REQUIRE(tokens[1].sym == "id");
  REQUIRE(tokens[2].sym == "=");
  REQUIRE(tokens[3].sym == "lit");
  REQUIRE(tokens[4].sym == ";");
}

TEST_CASE("Token normalizer: oversized files are skipped (#147)", "[duplication]")
{
  // A multi-MB embedded-data file (e.g. a vocab `.hpp` of byte literals) must not be
  // tokenized — doing so builds a multi-GB token + k-gram blow-up (archcheck RSS > 3 GB).
  // lex returns no tokens past kMaxLexBytes so every lex-based scan skips the file.
  const std::string unit = "int x = 5;\n"; // 11 tokens-worth per copy
  std::string big;
  big.reserve(kMaxLexBytes + unit.size() * 2);
  while (big.size() <= kMaxLexBytes)
    big += unit;
  REQUIRE(big.size() > kMaxLexBytes);
  REQUIRE(lex(big).empty()); // over the cap -> no tokens

  std::string under;
  while (under.size() < kMaxLexBytes - unit.size())
    under += unit;
  REQUIRE(under.size() <= kMaxLexBytes);
  REQUIRE_FALSE(lex(under).empty()); // just under the cap -> still tokenized
}

TEST_CASE("Token normalizer: comment handling", "[duplication]")
{
  const std::string src = "int x; // comment\nint y;";
  const auto tokens = lex(src);

  REQUIRE(tokens.size() == 6);
  REQUIRE(tokens[0].line == 1);
  REQUIRE(tokens[3].line == 2); // "int" on line 2
  REQUIRE(tokens[5].line == 2); // ";" on line 2
}

TEST_CASE("Token normalizer: string literal", "[duplication]")
{
  const std::string src = R"(const char* msg = "hello";)";
  const auto tokens = lex(src);

  bool found = false;
  for (const auto &t : tokens)
  {
    if (t.sym == "lit" && t.raw == R"("hello")")
    {
      found = true;
      break;
    }
  }
  REQUIRE(found);
}

TEST_CASE("Token normalizer: keepCalls=true preserves function names", "[duplication]")
{
  const std::string src = "void func() { call(x); }";
  const auto tokens = lex(src, true);

  bool foundCall = false;
  for (const auto &t : tokens)
  {
    if (t.sym == "call") // should be preserved, not "id"
    {
      foundCall = true;
      break;
    }
  }
  REQUIRE(foundCall);
}

TEST_CASE("Token normalizer: keepCalls=false normalizes all identifiers", "[duplication]")
{
  const std::string src = "void func() { call(x); }";
  const auto tokens = lex(src, false);

  bool foundCall = false;
  for (const auto &t : tokens)
  {
    if (t.sym == "call")
    {
      foundCall = true;
      break;
    }
  }
  REQUIRE(!foundCall);
}

// A stray single quote swallows text up to the next quote, possibly lines away.
// The pre-unification char path did not advance `line` across that run, shifting
// the line numbers of every later token (and file:line is the product contract).
// tryConsumeQuoted counts raw newlines for both quote kinds; this pins it.
TEST_CASE("Token normalizer: newline inside a quoted run keeps later line numbers", "[duplication]")
{
  const std::string src = "int a;\n' x\ny '\nint b;\n";
  const auto tokens = lex(src, true);

  bool checked = false;
  for (const auto &t : tokens)
  {
    if (t.sym == "id" && t.raw == "b")
    {
      REQUIRE(t.line == 4);
      checked = true;
    }
  }
  REQUIRE(checked);
}

// A `#if 0` block is dead code and must not be lexed, but its `#else`/`#elif` alternative
// is LIVE and must be. skipDeadIfBlock tracked only #if/#endif depth, so it ran past the
// #else and swallowed the live branch — manufacturing recall gaps against line-based
// oracles that do not preprocess (nanovdb Ray.h intersects() clone, #201). Nested #if in
// the dead branch stays fully skipped.
TEST_CASE("Token normalizer: #if 0 skips dead branch but keeps the #else branch (#201)", "[duplication]")
{
  const std::string src = "int a;\n"
                          "#if 0\n"
                          "  DEAD deadCall();\n"
                          "#if NESTED\n"
                          "  STILL_DEAD;\n"
                          "#endif\n"
                          "#else\n"
                          "  int liveVar = 7;\n"
                          "#endif\n"
                          "int b;\n";
  const auto tokens = lex(src, true);

  const auto hasRaw = [&](const std::string &r)
  { return std::any_of(tokens.begin(), tokens.end(), [&](const Token &t) { return t.raw == r; }); };

  REQUIRE_FALSE(hasRaw("DEAD"));
  REQUIRE_FALSE(hasRaw("STILL_DEAD"));
  REQUIRE(hasRaw("liveVar"));

  // The live token and the trailing code keep their true source line numbers.
  for (const auto &t : tokens)
    if (t.raw == "liveVar")
      REQUIRE(t.line == 8);
  for (const auto &t : tokens)
    if (t.raw == "b")
      REQUIRE(t.line == 10);
}

// A backslash-newline line continuation inside a string literal ("...\n\<newline>...) is a
// physical line and must be counted, or every later token's reported line drifts. openvdb
// ParmFactory.cc has an ~18-line continued help string; the drift reported real clones tens
// of lines off, so they failed to match line-based oracles (Duplo) — read as recall misses
// (#201). file:line is the product contract.
TEST_CASE("Token normalizer: backslash-newline in a string keeps later line numbers (#201)", "[duplication]")
{
  const std::string src = "int a;\n"
                          "const char* s = \"line one\\n\\\n"
                          "line two\\n\\\n"
                          "line three\\n\";\n"
                          "int later;\n";
  const auto tokens = lex(src, true);

  bool checked = false;
  for (const auto &t : tokens)
    if (t.raw == "later")
    {
      REQUIRE(t.line == 5); // int a=1, string spans 2-4, later=5
      checked = true;
    }
  REQUIRE(checked);
}
