#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "archcheck/scan/duplication/duplication_scanner.h" // Fragment, Pair

namespace archcheck::scan::duplication
{

struct RunMatchOptions
{
  std::size_t minTokens = 30;   // report spans of at least this many aligned tokens (both sides)
  int minLines = 0;             // additionally require this many source lines each side (0 = off)
  std::size_t seedK = 20;       // k-gram seed length; clamped to <= minTokens internally
  int gapTokens = 30;           // Type-3 stitch: largest edit gap between two co-linear runs
  std::size_t bucketCap = 1000; // skip idiom seeds shared by more than this many positions
  // Rarity/idiom guard (SSOT §3.3 relative rare-df / P1.4): a real copy shares
  // project-specific tokens; a boilerplate idiom (namespace preambles, interface signatures)
  // shares only ubiquitous ones. A span must carry >= minRareAnchors distinct tokens whose
  // document frequency is <= rareDfFrac of the file count. 0 = off (no rarity filtering).
  int minRareAnchors = 0;
  double rareDfFrac = 0.12;
  // Structural idiom guard: require the span to contain executable control flow
  // (if/for/while/switch/do/catch/return). The idiom-FP classes a window-free core surfaces
  // — namespace-preamble declaration runs, interface signatures, literal data tables — are
  // all NON-executable; a real copied body has control flow. Unlike the rarity guard, this
  // spares the "parameterized writer/helper" TP class (which shares no rare token but does
  // branch/loop). true = drop control-flow-free spans.
  bool requireControlFlow = false;
};

// Window-free clone core (#199 / ADR-004): maximal matching runs over the *normalized*
// token stream (reusing lex() unchanged), stitched into Type-3 spans, filtered by a
// minimum size that is a pure post-filter — so min-size is monotonic by construction.
// Emits each span's two endpoints as Fragments (only file/startLine/endLine populated)
// and Pairs indexing into them, so the existing reporter and --diff path consume the
// output with no change. No windowing, no fragmenter: sub-function, cross-function, and
// trailing-qualifier-method clones are native runs (closes the #190/#191/#195/#196 class).
void matchTokenRuns(const std::vector<std::pair<std::string, std::string>> &files, const RunMatchOptions &opts,
                    std::vector<Fragment> &outFragments, std::vector<Pair> &outPairs);

} // namespace archcheck::scan::duplication
