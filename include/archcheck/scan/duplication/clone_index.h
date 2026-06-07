#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace archcheck::scan::duplication
{

struct Fragment;

struct IndexOptions
{
  std::size_t rareDfCap = 4;     // document frequency cap for "rare" tokens
  double rareDfPct = 0.0;        // if >0, rare cutoff = max(rareDfCap, N*pct/100)
  std::size_t minSharedRare = 2; // minimum rare tokens to form a candidate pair
  double minDiversity = 0.0;     // min trigram diversity; filter skeletal fragments
  // #092: scale-independent candidate generation. The rare-token index above keys
  // on corpus document-frequency, so a genuine clone pair stops being a candidate
  // once the project grows enough that its shared tokens are no longer "rare"
  // (a copy-paste is a copy-paste at any project size). k-gram winnowing
  // fingerprints are intrinsic to each fragment's token sequence: two fragments
  // sharing a fingerprinted run become candidates regardless of corpus size.
  // Additive — unions with the rare-token candidates.
  bool fingerprints = true;       // enable k-gram winnowing candidate generation
  std::size_t fpK = 5;            // k-gram length (tokens)
  std::size_t fpWindow = 8;       // winnowing window; detects shared runs >= fpK+fpWindow-1
  std::size_t fpMaxPostings = 20; // drop a fingerprint shared by > this many fragments
                                  // (a distinctive clone run is rare in absolute terms at
                                  // any corpus size; an over-frequent run is boilerplate idiom)
};

struct CloneIndex
{
  std::unordered_map<std::string, int> df;                               // token -> document frequency
  std::unordered_map<std::string, double> idf;                           // token -> IDF weight
  std::unordered_map<std::string, std::vector<std::size_t>> postings;    // token -> fragment indices
  std::map<std::pair<std::size_t, std::size_t>, std::size_t> sharedRare; // (fi, fj) -> shared rare token count
};

// Build inverted index from fragments: compute df/idf, postings, and candidate pairs.
CloneIndex buildIndex(const std::vector<Fragment> &fragments, const IndexOptions &opts = IndexOptions{});

} // namespace archcheck::scan::duplication
