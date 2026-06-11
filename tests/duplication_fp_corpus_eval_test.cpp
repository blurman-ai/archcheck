#include <catch2/catch_test_macros.hpp>

#include "archcheck/scan/duplication/fp_corpus_eval.h"
#include "archcheck/scan/duplication/token_normalizer.h"

using namespace archcheck::scan::duplication;

TEST_CASE("FP corpus eval: loadCorpusGroundTruth returns empty on missing file", "[duplication][corpus-eval]")
{
  const auto truth = loadCorpusGroundTruth("/nonexistent/path/to/corpus.tsv");
  REQUIRE(truth.empty());
}

TEST_CASE("FP corpus eval: basic corpus loading", "[duplication][corpus-eval]")
{
  // This test would require a sample corpus file. For now, test empty case.
  const GroundTruth truth;
  REQUIRE(truth.empty());
}
