#include <catch2/catch_test_macros.hpp>

#include "archcheck/git/git_state.h"

using archcheck::git::parseRevspec;
using archcheck::git::Revspec;

TEST_CASE("parseRevspec accepts a..b", "[git][revspec]")
{
  const auto r = parseRevspec("main..HEAD");
  REQUIRE(r.has_value());
  REQUIRE(r->baseline == "main");
  REQUIRE(r->current == "HEAD");
}

TEST_CASE("parseRevspec treats single ref as <ref>..WORKTREE", "[git][revspec]")
{
  const auto r = parseRevspec("HEAD~1");
  REQUIRE(r.has_value());
  REQUIRE(r->baseline == "HEAD~1");
  REQUIRE(r->current == "WORKTREE");
}

TEST_CASE("parseRevspec accepts SHA range", "[git][revspec]")
{
  const auto r = parseRevspec("abc1234..def5678");
  REQUIRE(r.has_value());
  REQUIRE(r->baseline == "abc1234");
  REQUIRE(r->current == "def5678");
}

TEST_CASE("parseRevspec rejects empty input", "[git][revspec]") { REQUIRE_FALSE(parseRevspec("").has_value()); }

TEST_CASE("parseRevspec rejects empty sides", "[git][revspec]")
{
  REQUIRE_FALSE(parseRevspec("..HEAD").has_value());
  REQUIRE_FALSE(parseRevspec("main..").has_value());
}

TEST_CASE("parseRevspec rejects three-dot symmetric form", "[git][revspec]")
{
  // We deliberately only support the two-dot form; '...' is reserved.
  REQUIRE_FALSE(parseRevspec("main...HEAD").has_value());
}
