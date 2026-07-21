#include "archcheck/scan/duplication/clone_index.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <unordered_set>

#include "archcheck/scan/duplication/fragmenter.h"

namespace archcheck::scan::duplication
{

namespace
{
// Nested fragments (#190) are excluded: their tokens are already counted in the
// enclosing body, so counting them again double-counts the same text and drives IDF
// toward zero. Returns the number of documents actually counted.
std::size_t computeDocumentFrequency(const std::vector<Fragment> &fragments, std::unordered_map<std::string, int> &df)
{
  std::size_t documents = 0;
  for (const auto &frag : fragments)
  {
    if (frag.nested)
    {
      continue;
    }
    ++documents;
    for (const auto &[sym, _] : frag.bag)
    {
      ++df[sym];
    }
  }
  return documents;
}

void computeIdfWeights(const std::size_t fragmentCount, std::unordered_map<std::string, int> &df,
                       std::unordered_map<std::string, double> &idf)
{
  for (auto &[sym, d] : df)
  {
    idf[sym] = std::log(static_cast<double>(fragmentCount) / static_cast<double>(d));
  }
}

std::size_t getEffectiveRareDf(const std::size_t fragmentCount, const IndexOptions &opts)
{
  std::size_t effRareDf = opts.rareDfCap;
  if (opts.rareDfPct > 0.0)
  {
    const std::size_t pctCap = static_cast<std::size_t>(fragmentCount * opts.rareDfPct / 100.0);
    effRareDf = std::max(opts.rareDfCap, pctCap);
  }
  return effRareDf;
}

void buildRareTokenIndex(const std::vector<Fragment> &fragments, std::size_t effRareDf,
                         const std::unordered_map<std::string, int> &df,
                         std::unordered_map<std::string, std::vector<std::size_t>> &postings)
{
  for (std::size_t fi = 0; fi < fragments.size(); ++fi)
  {
    for (const auto &[sym, cnt] : fragments[fi].bag)
    {
      // #191: a token can live only in statement-run fragments whose oversized
      // enclosing body was never emitted as a document, so it is absent from df.
      // Absent = zero documents = maximally rare; index it (df.find, not df.at).
      const auto d = df.find(sym);
      const std::size_t docFreq = d == df.end() ? 0 : static_cast<std::size_t>(d->second);
      if (docFreq <= effRareDf)
      {
        postings[sym].push_back(fi);
      }
    }
  }
}

// A same-file candidate pair whose spans overlap or merely abut is never actionable
// copy-paste (a parent/child fragment, or two abutting windows of the same site) and
// the scanner's phase7 drops it unconditionally. Skipping it at candidate generation
// keeps #191's heavily-overlapping statement-run windows from forming an O(k^2)
// same-function clique that would otherwise be scored and only then discarded.
bool sameFileOverlapOrAdjacent(const Fragment &a, const Fragment &b)
{
  if (a.file != b.file || a.startLine <= 0 || b.startLine <= 0)
  {
    return false; // different files, or unset coordinates — cannot assert overlap
  }
  const bool overlap = a.startLine <= b.endLine && b.startLine <= a.endLine;
  const bool adjacent = a.endLine + 1 == b.startLine || b.endLine + 1 == a.startLine;
  return overlap || adjacent;
}

void findCandidatePairs(const std::vector<Fragment> &fragments,
                        const std::unordered_map<std::string, std::vector<std::size_t>> &postings,
                        std::map<std::pair<std::size_t, std::size_t>, std::size_t> &sharedRare)
{
  for (const auto &[sym, list] : postings)
  {
    for (std::size_t x = 0; x < list.size(); ++x)
    {
      for (std::size_t y = x + 1; y < list.size(); ++y)
      {
        if (sameFileOverlapOrAdjacent(fragments[list[x]], fragments[list[y]]))
        {
          continue;
        }
        const auto key = std::make_pair(std::min(list[x], list[y]), std::max(list[x], list[y]));
        ++sharedRare[key];
      }
    }
  }
}

bool shouldKeepPair(const std::pair<std::size_t, std::size_t> &pair, std::size_t shared,
                    const std::vector<Fragment> &fragments, const IndexOptions &opts)
{
  if (shared < opts.minSharedRare)
  {
    return false;
  }
  if (opts.minDiversity > 0.0)
  {
    const auto [a, b] = pair;
    if (std::min(fragments[a].diversity, fragments[b].diversity) < opts.minDiversity)
    {
      return false;
    }
  }
  return true;
}

void filterCandidatePairs(std::map<std::pair<std::size_t, std::size_t>, std::size_t> &sharedRare,
                          const std::vector<Fragment> &fragments, const IndexOptions &opts)
{
  auto it = sharedRare.begin();
  while (it != sharedRare.end())
  {
    if (shouldKeepPair(it->first, it->second, fragments, opts))
    {
      ++it;
    }
    else
    {
      it = sharedRare.erase(it);
    }
  }
}

// FNV-1a hash of every fpK-gram of a token sequence (empty if shorter than k).
std::vector<std::uint64_t> kGramHashes(const std::vector<std::string> &seq, std::size_t k)
{
  std::vector<std::uint64_t> grams;
  if (seq.size() < k)
  {
    return grams;
  }
  grams.reserve(seq.size() - k + 1);
  for (std::size_t i = 0; i + k <= seq.size(); ++i)
  {
    std::uint64_t h = 1469598103934665603ULL;
    for (std::size_t j = i; j < i + k; ++j)
    {
      h = (h ^ std::hash<std::string>{}(seq[j])) * 1099511628211ULL;
    }
    grams.push_back(h);
  }
  return grams;
}

// #092: robust winnowing fingerprints — keep the min hash of each fpWindow,
// re-emitting only when the window minimum changes (density ~1/fpWindow,
// intrinsic to content, so the candidacy of a clone pair is corpus-size-independent).
std::unordered_set<std::uint64_t> fingerprintsOf(const std::vector<std::string> &seq, std::size_t k, std::size_t w)
{
  std::unordered_set<std::uint64_t> fps;
  const std::vector<std::uint64_t> grams = kGramHashes(seq, k);
  std::size_t minPos = grams.size(); // sentinel: no min emitted yet
  for (std::size_t end = w == 0 ? 0 : w - 1; end < grams.size(); ++end)
  {
    const std::size_t start = end + 1 - w;
    std::size_t best = start;
    for (std::size_t p = start + 1; p <= end; ++p)
    {
      if (grams[p] <= grams[best])
      {
        best = p; // rightmost-min on ties
      }
    }
    if (best != minPos)
    {
      fps.insert(grams[best]);
      minPos = best;
    }
  }
  return fps;
}

// Pair up every fragment sharing one fingerprint, bumping the shared count to the
// candidacy floor (a fingerprint match alone qualifies — a verbatim run of
// fpK+fpWindow-1 tokens is stronger than two shared rare tokens).
void pairFingerprintPostings(const std::vector<std::size_t> &list, const std::vector<Fragment> &fragments,
                             const IndexOptions &opts,
                             std::map<std::pair<std::size_t, std::size_t>, std::size_t> &sharedRare)
{
  for (std::size_t x = 0; x < list.size(); ++x)
  {
    for (std::size_t y = x + 1; y < list.size(); ++y)
    {
      if (sameFileOverlapOrAdjacent(fragments[list[x]], fragments[list[y]]))
      {
        continue;
      }
      const auto key = std::make_pair(std::min(list[x], list[y]), std::max(list[x], list[y]));
      auto &c = sharedRare[key];
      c = std::max(c, opts.minSharedRare); // ensure it clears shouldKeepPair's count gate
    }
  }
}

// Add candidate pairs whose fragments share a fingerprint.
void addFingerprintCandidates(const std::vector<Fragment> &fragments, const IndexOptions &opts,
                              std::map<std::pair<std::size_t, std::size_t>, std::size_t> &sharedRare)
{
  std::unordered_map<std::uint64_t, std::vector<std::size_t>> fpPostings;
  for (std::size_t fi = 0; fi < fragments.size(); ++fi)
  {
    for (std::uint64_t fp : fingerprintsOf(fragments[fi].seq, opts.fpK, opts.fpWindow))
    {
      fpPostings[fp].push_back(fi);
    }
  }
  for (const auto &[fp, list] : fpPostings)
  {
    // Count documents, not fragments (#190): a run inside a body is fingerprinted both
    // in that body and in the nested block, so counting raw postings double-counts the
    // same text and pushes ordinary runs over the cap — that silenced whole families of
    // real clones (duckdb's duckdb_tables/views/indexes). Nested fragments still pair up
    // below; they just do not inflate the frequency judgement.
    const std::size_t documents = static_cast<std::size_t>(
        std::count_if(list.begin(), list.end(), [&](std::size_t fi) { return !fragments[fi].nested; }));
    if (opts.fpMaxPostings > 0 && documents > opts.fpMaxPostings)
    {
      continue; // over-frequent fingerprint = boilerplate idiom, not a clone signal
    }
    pairFingerprintPostings(list, fragments, opts, sharedRare);
  }
}
} // namespace

CloneIndex buildIndex(const std::vector<Fragment> &fragments, const IndexOptions &opts)
{
  CloneIndex idx;

  // N counts documents, not fragments: nested blocks (#190) share their text with the
  // enclosing body and must not shift the df/idf scale the guards are calibrated on.
  const std::size_t N = computeDocumentFrequency(fragments, idx.df);
  computeIdfWeights(N, idx.df, idx.idf);
  std::size_t effRareDf = getEffectiveRareDf(N, opts);
  buildRareTokenIndex(fragments, effRareDf, idx.df, idx.postings);
  findCandidatePairs(fragments, idx.postings, idx.sharedRare);
  if (opts.fingerprints)
  {
    addFingerprintCandidates(fragments, opts, idx.sharedRare);
  }
  filterCandidatePairs(idx.sharedRare, fragments, opts);

  return idx;
}

} // namespace archcheck::scan::duplication
