#include <catch2/catch_test_macros.hpp>

#include "archcheck/scan/duplication/clone_classifier.h"

using namespace archcheck::scan::duplication;

TEST_CASE("Clone type: EXACT identical", "[duplication]")
{
  Fragment a, b;
  a.seq = {"int", "x", "=", "lit", ";"};
  b.seq = {"int", "x", "=", "lit", ";"};
  a.rawSeq = {"int", "x", "=", "5", ";"};
  b.rawSeq = {"int", "x", "=", "5", ";"};

  const char *type = cloneType(a, b);
  REQUIRE(std::string(type) == "EXACT");
}

TEST_CASE("Clone type: RENAMED identifiers differ", "[duplication]")
{
  Fragment a, b;
  a.seq = {"int", "id", "=", "lit", ";"};
  b.seq = {"int", "id", "=", "lit", ";"};
  a.rawSeq = {"int", "x", "=", "5", ";"};
  b.rawSeq = {"int", "y", "=", "5", ";"};

  const char *type = cloneType(a, b);
  REQUIRE(std::string(type) == "RENAMED");
}

TEST_CASE("Clone type: LITERAL numbers differ", "[duplication]")
{
  Fragment a, b;
  a.seq = {"int", "x", "=", "lit", ";"};
  b.seq = {"int", "x", "=", "lit", ";"};
  a.rawSeq = {"int", "x", "=", "5", ";"};
  b.rawSeq = {"int", "x", "=", "10", ";"};

  const char *type = cloneType(a, b);
  REQUIRE(std::string(type) == "LITERAL");
}

TEST_CASE("Clone type: STRUCTURAL normalized streams differ", "[duplication]")
{
  Fragment a, b;
  a.seq = {"int", "x", "=", "id", "(", ")", ";"};
  b.seq = {"int", "x", "=", "+", "lit", ";"};

  const char *type = cloneType(a, b);
  REQUIRE(std::string(type) == "STRUCTURAL");
}
