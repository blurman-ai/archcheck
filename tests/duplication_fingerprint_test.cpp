#include <catch2/catch_test_macros.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "archcheck/scan/duplication/clone_index.h"
#include "archcheck/scan/duplication/fragmenter.h"

using namespace archcheck::scan::duplication;

// #092: candidate generation must be scale-independent. The rare-token index keys
// on corpus document-frequency, so a genuine clone pair stops being a candidate
// once the project is large enough that its shared tokens are no longer "rare".
// k-gram winnowing fingerprints are intrinsic to the two fragments' token run, so
// the pair is a candidate at any corpus size. This reproduces the bug with a real
// clone whose vocabulary is made corpus-common by filler fragments.
TEST_CASE("CloneIndex: clone pair stays a candidate regardless of corpus size (#092)", "[duplication][fingerprints]")
{
  // A distinctive 25-token run shared verbatim by two fragments (the clone).
  std::vector<std::string> run;
  std::unordered_map<std::string, int> runBag;
  for (int i = 0; i < 25; ++i)
  {
    run.push_back("s" + std::to_string(i));
    runBag["s" + std::to_string(i)] = 1;
  }

  Fragment a, b;
  a.seq = run;
  a.bag = runBag;
  b.seq = run;
  b.bag = runBag;
  std::vector<Fragment> frags = {a, b};

  // 12 filler fragments that CONTAIN every run token (pushing each token's
  // document-frequency well past rareDfCap) but never as a consecutive run — each
  // run token is separated by a filler-unique token, so no fingerprint is shared.
  for (int f = 0; f < 12; ++f)
  {
    Fragment filler;
    filler.bag = runBag;
    for (int i = 0; i < 25; ++i)
    {
      filler.seq.push_back(run[i]);
      filler.seq.push_back("u" + std::to_string(f) + "_" + std::to_string(i));
    }
    frags.push_back(filler);
  }

  const std::pair<std::size_t, std::size_t> clonePair{0, 1};

  SECTION("rare-token index alone: shared tokens are now common -> NOT a candidate (the bug)")
  {
    IndexOptions opts;
    opts.fingerprints = false;
    const auto idx = buildIndex(frags, opts);
    REQUIRE(idx.sharedRare.count(clonePair) == 0);
  }

  SECTION("fingerprints: the shared run is intrinsic -> still a candidate (the fix)")
  {
    IndexOptions opts;
    opts.fingerprints = true;
    const auto idx = buildIndex(frags, opts);
    REQUIRE(idx.sharedRare.count(clonePair) == 1);
  }
}
