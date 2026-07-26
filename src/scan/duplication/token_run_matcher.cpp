#include "archcheck/scan/duplication/token_run_matcher.h"

#include <algorithm>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "archcheck/scan/duplication/token_normalizer.h"

namespace archcheck::scan::duplication
{
namespace
{

struct StreamTok
{
  int fileIdx;            // owning file, or -1 for a between-files sentinel
  int line;               // 1-based source line
  int sym;                // interned normalized symbol (negative = unique sentinel)
  const std::string *raw; // raw spelling for classification (null only for sentinels)
  unsigned char kind;     // 0 concrete token, 1 = "id" placeholder, 2 = "lit" placeholder
};

struct Run
{
  int a;   // stream position of the first copy
  int b;   // stream position of the second copy (a < b)
  int len; // aligned token length
};

struct Span
{
  int a, aEnd; // token range [a, aEnd) of the first copy
  int b, bEnd; // token range [b, bEnd) of the second copy
  int matched; // aligned (exactly-matching) tokens inside
  int runs;    // maximal runs stitched together (1 = a single unbroken run)
};

using Buckets = std::unordered_map<std::uint64_t, std::vector<int>>;

// Lex every file, mask preprocessor directives (an #include preamble normalizes to the
// same `# id < id >` skeleton and would flood the output with shifted ladders — the
// fragmenter was an implicit scope filter the run core lacks), and separate files with a
// unique sentinel that can never match across the boundary. Raw pointers stay valid: the
// outer vector is reserved, so inner token vectors never move.
void buildStream(const std::vector<std::pair<std::string, std::string>> &files, std::vector<StreamTok> &stream,
                 std::vector<std::string> &names, std::vector<std::vector<Token>> &owned)
{
  std::unordered_map<std::string, int> symTab;
  auto intern = [&](const std::string &s) { return symTab.emplace(s, static_cast<int>(symTab.size())).first->second; };
  owned.reserve(files.size());
  int sentinel = 1;
  for (const auto &[path, source] : files)
  {
    names.push_back(path);
    owned.push_back(lex(source, /*keepCalls=*/true));
    const int f = static_cast<int>(names.size()) - 1;
    int ppLine = -1;
    for (const auto &t : owned.back())
    {
      if (t.sym == "#")
        ppLine = t.line;
      if (t.line == ppLine)
        continue;
      const unsigned char kind = t.sym == "id" ? 1 : (t.sym == "lit" ? 2 : 0);
      stream.push_back({f, t.line, intern(t.sym), t.raw.empty() ? &t.sym : &t.raw, kind});
    }
    stream.push_back({-1, 0, -(sentinel++), nullptr, 0}); // never matches (fileIdx < 0)
  }
}

// Rolling-hash every k-gram of the stream into buckets of start positions.
Buckets seedBuckets(const std::vector<StreamTok> &stream, std::size_t k)
{
  Buckets buckets;
  const std::size_t n = stream.size();
  if (n < k)
    return buckets;
  const std::uint64_t base = 1000003ULL;
  auto val = [&](std::size_t i) { return static_cast<std::uint64_t>(stream[i].sym) + 0x9E3779B9ULL; };
  std::uint64_t pow = 1;
  for (std::size_t i = 0; i + 1 < k; ++i)
    pow *= base;
  std::uint64_t h = 0;
  for (std::size_t i = 0; i < k; ++i)
    h = h * base + val(i);
  buckets[h].push_back(0);
  for (std::size_t i = k; i < n; ++i)
  {
    h = (h - val(i - k) * pow) * base + val(i);
    buckets[h].push_back(static_cast<int>(i - k + 1));
  }
  return buckets;
}

bool sameSym(const std::vector<StreamTok> &s, int x, int y) { return s[x].fileIdx >= 0 && s[x].sym == s[y].sym; }

// Verify a seed hit (guard against a hash collision) then extend it maximally both ways.
// The forward reach is capped at the pair distance (a+len <= b) so a same-file run never
// overlaps its own second copy. Returns len 0 when the seed does not actually match.
Run extendPair(const std::vector<StreamTok> &s, int a, int b, std::size_t k)
{
  for (std::size_t d = 0; d < k; ++d)
    if (!sameSym(s, a + static_cast<int>(d), b + static_cast<int>(d)))
      return {a, b, 0};
  while (a > 0 && b - 1 > a && sameSym(s, a - 1, b - 1))
    --a, --b;
  int len = static_cast<int>(k);
  const int n = static_cast<int>(s.size());
  while (a + len < n && b + len < n && a + len < b && sameSym(s, a + len, b + len))
    ++len;
  return {a, b, len};
}

// Expand every position pair in every non-idiom bucket into a maximal run. The std::set
// dedupes and orders the runs, so the result is independent of bucket iteration order.
void extendRuns(const std::vector<StreamTok> &stream, const Buckets &buckets, std::size_t k, std::size_t cap,
                std::vector<Run> &out)
{
  std::set<std::tuple<int, int, int>> seen;
  for (const auto &[h, pos] : buckets)
  {
    if (pos.size() < 2 || pos.size() > cap)
      continue;
    for (std::size_t i = 0; i < pos.size(); ++i)
      for (std::size_t j = i + 1; j < pos.size(); ++j)
      {
        const Run r = extendPair(stream, pos[i], pos[j], k);
        if (r.len > 0)
          seen.emplace(r.a, r.b, r.len);
      }
  }
  for (const auto &[a, b, len] : seen)
    out.push_back({a, b, len});
}

// Drop a run contained in a longer run on the same diagonal (they overlap identically).
// Containment can only hold on the same diagonal (a-b), so kept runs are indexed by diagonal
// as start->end interval maps: a new run tests just the one interval that could contain it
// (the predecessor by start), not every kept run. Self-similar data tables put ~n runs on one
// diagonal, so the old O(n^2) linear scan was the openvdb blow-up (#201); this is O(n log n)
// and keeps the identical set (same len-desc order, same containment test).
void dedupRuns(std::vector<Run> &runs)
{
  std::sort(runs.begin(), runs.end(),
            [](const Run &x, const Run &y) { return std::tie(y.len, x.a, x.b) < std::tie(x.len, y.a, y.b); });
  std::unordered_map<int, std::map<int, int>> byDiag; // diagonal (a-b) -> (start a -> end a)
  std::vector<Run> kept;
  for (const auto &r : runs)
  {
    auto &ivals = byDiag[r.a - r.b];
    const auto it = ivals.upper_bound(r.a); // kept intervals are start- and end-monotonic, so
    const bool contained =                  // only the predecessor by start can cover [a, a+len)
        it != ivals.begin() && std::prev(it)->second >= r.a + r.len;
    if (!contained)
    {
      ivals[r.a] = r.a + r.len;
      kept.push_back(r);
    }
  }
  runs.swap(kept);
}

// A run stitches onto a span when it continues the same diagonal within a small edit gap.
// Runs on both sides of a point edit overrun it and OVERLAP, so a small negative gap is the
// normal case. The tandem guard stops a periodic twin family (A~B~C adjacent in one file)
// from chain-fusing into one self-overlapping span.
bool canStitch(const Span &s, const Run &r, const std::vector<StreamTok> &stream, int minGap, int maxGap)
{
  const int gapA = r.a - s.aEnd, gapB = r.b - s.bEnd;
  if (gapA < minGap || gapA > maxGap || gapB < minGap || gapB > maxGap)
    return false;
  if (r.a + r.len <= s.aEnd && r.b + r.len <= s.bEnd)
    return false; // fully inside — nothing to extend
  if (stream[r.a].fileIdx != stream[s.aEnd - 1].fileIdx || stream[r.b].fileIdx != stream[s.bEnd - 1].fileIdx)
    return false; // must stay within the same two files
  const bool sameFile = stream[r.a].fileIdx == stream[r.b].fileIdx;
  return !(sameFile && std::max(s.aEnd, r.a + r.len) > s.b);
}

void absorb(Span &s, const Run &r)
{
  s.matched += std::min(r.len, std::min(r.a + r.len - s.aEnd, r.b + r.len - s.bEnd));
  s.aEnd = std::max(s.aEnd, r.a + r.len);
  s.bEnd = std::max(s.bEnd, r.b + r.len);
  ++s.runs;
}

int spanDiag(const Span &s) { return s.aEnd - s.bEnd; }

using DiagIndex = std::unordered_map<int, std::vector<int>>; // (aEnd-bEnd) -> span indices, stale after absorb

// Highest-index span that stitches onto run r, or -1. canStitch forces the host's diagonal
// within (maxGap-minGap) of the run's, so only that diagonal window is scanned (not every span);
// stale entries (a span moved diagonal after absorb) are re-checked by spanDiag and skipped.
int findStitchHost(const DiagIndex &byDiag, const std::vector<Span> &spans, const Run &r,
                   const std::vector<StreamTok> &stream, std::pair<int, int> gaps)
{
  const int dr = r.a - r.b, band = gaps.second - gaps.first;
  int host = -1;
  for (int d = dr - band; d <= dr + band; ++d)
  {
    const auto it = byDiag.find(d);
    if (it == byDiag.end())
      continue;
    for (auto j = it->second.rbegin(); j != it->second.rend() && *j > host; ++j)
      if (spanDiag(spans[*j]) == d && canStitch(spans[*j], r, stream, gaps.first, gaps.second))
      {
        host = *j; // reverse order: first stitchable here is this bucket's highest index
        break;
      }
  }
  return host;
}

// Merge co-linear runs into Type-3 spans. Spans are indexed by growing diagonal so each run
// scans only the compatible diagonal band, not every span — the picked host is the same one the
// old full reverse scan chose, so the result is identical. Self-similar regions spread spans
// across many diagonals, so the old O(runs*spans) scan was the openvdb blow-up (#201).
void stitchSpans(std::vector<Run> &runs, const std::vector<StreamTok> &stream, int gap, std::size_t k,
                 std::vector<Span> &spans)
{
  std::sort(runs.begin(), runs.end(),
            [](const Run &x, const Run &y) { return std::tie(x.a, x.b, x.len) < std::tie(y.a, y.b, y.len); });
  const int minGap = -static_cast<int>(k);
  DiagIndex byDiag;
  for (const auto &r : runs)
  {
    const int host = findStitchHost(byDiag, spans, r, stream, {minGap, gap});
    if (host >= 0)
    {
      const int oldD = spanDiag(spans[host]);
      absorb(spans[host], r);
      if (spanDiag(spans[host]) != oldD)
        byDiag[spanDiag(spans[host])].push_back(host);
    }
    else
    {
      spans.push_back({r.a, r.a + r.len, r.b, r.b + r.len, r.len, 1});
      byDiag[spanDiag(spans.back())].push_back(static_cast<int>(spans.size()) - 1);
    }
  }
}

int spanTokens(const Span &s) { return (s.aEnd - s.a) + (s.bEnd - s.b); }

// Drop a span contained in another span on both sides (a stitched superset already covers it).
// Both-side containment forces the two spans into the same (fileA, fileB) pair, so a span tests
// only its own file-pair bucket, not every kept span. Self-similar regions push half a million
// spans through here on openvdb; the old global O(n^2) scan was 7+ minutes of the run (#201).
void dedupSpans(std::vector<Span> &spans, const std::vector<StreamTok> &stream)
{
  std::sort(spans.begin(), spans.end(), [](const Span &x, const Span &y)
            { return std::make_tuple(spanTokens(y), y.a, y.b) < std::make_tuple(spanTokens(x), x.a, x.b); });
  std::unordered_map<std::int64_t, std::vector<Span>> byFilePair;
  std::vector<Span> kept;
  for (const auto &s : spans)
  {
    const std::int64_t fp = (static_cast<std::int64_t>(stream[s.a].fileIdx) << 32) | stream[s.b].fileIdx;
    auto &bucket = byFilePair[fp];
    bool contained = false;
    for (const auto &k : bucket)
      if (s.a >= k.a && s.aEnd <= k.aEnd && s.b >= k.b && s.bEnd <= k.bEnd)
      {
        contained = true;
        break;
      }
    if (!contained)
    {
      bucket.push_back(s);
      kept.push_back(s);
    }
  }
  spans.swap(kept);
}

// Positions in an exact normalized run align 1:1, so classification is a direct raw-lexeme
// scan — no LCS needed. A stitched span carries interior edits, so it is MIXED.
std::string classify(const Span &s, const std::vector<StreamTok> &stream)
{
  if (s.runs > 1)
    return "MIXED";
  int idSubst = 0, litSubst = 0;
  for (int i = 0; i < s.aEnd - s.a; ++i)
    if (*stream[s.a + i].raw != *stream[s.b + i].raw)
      (stream[s.a + i].kind == 2 ? litSubst : idSubst)++;
  if (idSubst + litSubst == 0)
    return "EXACT";
  return litSubst > idSubst ? "LITERAL" : "RENAMED";
}

Fragment endpoint(const std::string &file, int start, int end)
{
  Fragment f;
  f.file = file;
  f.startLine = start;
  f.endLine = end;
  return f;
}

struct SpanEmit
{
  std::vector<Fragment> frags;
  std::vector<Pair> pairs;
};

// Document frequency per normalized symbol = number of DISTINCT files it appears in. The
// stream is grouped by file, so a last-seen-file marker counts distinct files in one pass.
// Placeholders (`id`, `lit`) and common keywords/operators land in nearly every file →
// high df → never counted as anchors, so no explicit exclude list is needed.
std::unordered_map<int, int> computeDf(const std::vector<StreamTok> &stream)
{
  std::unordered_map<int, int> df;
  std::unordered_map<int, int> lastFile;
  for (const auto &t : stream)
  {
    if (t.fileIdx < 0)
      continue;
    auto [it, inserted] = lastFile.emplace(t.sym, t.fileIdx);
    if (inserted || it->second != t.fileIdx)
    {
      ++df[t.sym];
      it->second = t.fileIdx;
    }
  }
  return df;
}

// Idiom guard: drop a span that shares fewer than minAnchors DISTINCT project-specific
// tokens (df <= cap). Kills namespace-preamble / interface-signature boilerplate that aligns
// only on ubiquitous tokens, while a real copy keeps its rare callees/identifiers.
void dropIdiomSpans(std::vector<Span> &spans, const std::vector<StreamTok> &stream,
                    const std::unordered_map<int, int> &df, int cap, int minAnchors)
{
  if (minAnchors <= 0)
    return;
  const auto rareAnchors = [&](const Span &s)
  {
    std::set<int> rare;
    for (int i = s.a; i < s.aEnd; ++i)
    {
      const auto it = df.find(stream[i].sym);
      if (stream[i].fileIdx >= 0 && it != df.end() && it->second <= cap)
        rare.insert(stream[i].sym);
    }
    return static_cast<int>(rare.size());
  };
  spans.erase(std::remove_if(spans.begin(), spans.end(), [&](const Span &s) { return rareAnchors(s) < minAnchors; }),
              spans.end());
}

// Structural idiom guard: drop a span with no executable control flow. The window-free
// idiom-FP classes (namespace-preamble declaration runs, interface signatures, literal data
// tables) are all non-executable; a real copied body branches or loops. Keywords keep their
// spelling (raw aliases sym), so a raw-lexeme membership test identifies them.
void dropStructurelessSpans(std::vector<Span> &spans, const std::vector<StreamTok> &stream, bool enabled)
{
  if (!enabled)
    return;
  static const std::set<std::string> kControlFlow = {"if", "for", "while", "switch", "do", "catch", "return"};
  const auto hasControlFlow = [&](const Span &s)
  {
    for (int i = s.a; i < s.aEnd; ++i)
      if (stream[i].fileIdx >= 0 && kControlFlow.count(*stream[i].raw) != 0)
        return true;
    return false;
  };
  spans.erase(std::remove_if(spans.begin(), spans.end(), [&](const Span &s) { return !hasControlFlow(s); }),
              spans.end());
}

// Post-filter by minimum size (a pure result filter → monotonic in min size) and
// materialize each span as a Pair over two endpoint Fragments the reporter already reads.
SpanEmit emitSpans(const std::vector<Span> &spans, const std::vector<StreamTok> &stream,
                   const std::vector<std::string> &names, const RunMatchOptions &opts)
{
  SpanEmit out;
  for (const auto &s : spans)
  {
    const int lenA = s.aEnd - s.a, lenB = s.bEnd - s.b;
    if (static_cast<std::size_t>(std::min(lenA, lenB)) < opts.minTokens)
      continue;
    const int a1 = stream[s.a].line, a2 = stream[s.aEnd - 1].line;
    const int b1 = stream[s.b].line, b2 = stream[s.bEnd - 1].line;
    if (opts.minLines > 0 && (a2 - a1 + 1 < opts.minLines || b2 - b1 + 1 < opts.minLines))
      continue;
    Pair p;
    p.a = out.frags.size();
    p.b = out.frags.size() + 1;
    p.weighted = static_cast<double>(s.matched) / std::max(lenA, lenB);
    p.line = p.weighted; // window-free: `line` mirrors the aligned-token fraction
    p.type = classify(s, stream);
    out.frags.push_back(endpoint(names[stream[s.a].fileIdx], a1, a2));
    out.frags.push_back(endpoint(names[stream[s.b].fileIdx], b1, b2));
    out.pairs.push_back(p);
  }
  return out;
}

} // namespace

void matchTokenRuns(const std::vector<std::pair<std::string, std::string>> &files, const RunMatchOptions &opts,
                    std::vector<Fragment> &outFragments, std::vector<Pair> &outPairs)
{
  std::vector<StreamTok> stream;
  std::vector<std::string> names;
  std::vector<std::vector<Token>> owned;
  buildStream(files, stream, names, owned);

  const std::size_t k = std::max<std::size_t>(8, std::min(opts.seedK, opts.minTokens));
  const Buckets buckets = seedBuckets(stream, k);
  std::vector<Run> runs;
  extendRuns(stream, buckets, k, opts.bucketCap, runs);
  dedupRuns(runs);

  std::vector<Span> spans;
  stitchSpans(runs, stream, opts.gapTokens, k, spans);
  // Idiom/structureless drops run BEFORE dedup: a rare-poor or control-flow-free span cannot
  // contain a rare-rich or control-flow-bearing one (its wider range would hold those tokens
  // too), so the surviving set is identical — but in the default (control-flow) mode the huge
  // data-table span flood is shed here, so dedup's work shrinks to the spans that can survive.
  const int dfCap = std::max(2, static_cast<int>(opts.rareDfFrac * static_cast<double>(names.size())));
  dropIdiomSpans(spans, stream, computeDf(stream), dfCap, opts.minRareAnchors);
  dropStructurelessSpans(spans, stream, opts.requireControlFlow);
  dedupSpans(spans, stream);
  SpanEmit emitted = emitSpans(spans, stream, names, opts);
  outFragments = std::move(emitted.frags);
  outPairs = std::move(emitted.pairs);
}

} // namespace archcheck::scan::duplication
