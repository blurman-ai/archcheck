#include "archcheck/scan/duplication/fragmenter.h"

#include <array>
#include <cctype>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace archcheck::scan::duplication
{

namespace
{

double trigramDiversity(const std::vector<std::string> &seq)
{
  if (seq.size() < 3)
  {
    return 1.0;
  }
  std::unordered_set<std::string> grams;
  for (std::size_t i = 0; i + 2 < seq.size(); ++i)
  {
    grams.insert(seq[i] + "\x01" + seq[i + 1] + "\x01" + seq[i + 2]);
  }
  return static_cast<double>(grams.size()) / static_cast<double>(seq.size() - 2);
}

std::vector<int> braceMatch(const std::vector<Token> &t)
{
  std::vector<int> match(t.size(), -1);
  std::vector<std::size_t> stack;
  for (std::size_t i = 0; i < t.size(); ++i)
  {
    if (t[i].sym == "{")
    {
      stack.push_back(i);
    }
    else if (t[i].sym == "}" && !stack.empty())
    {
      const std::size_t open = stack.back();
      stack.pop_back();
      match[open] = static_cast<int>(i);
      match[i] = static_cast<int>(open);
    }
  }
  return match;
}

std::string normalizeLine(const std::string &raw)
{
  std::string out;
  bool prevSpace = true;
  for (char c : raw)
  {
    if (std::isspace(static_cast<unsigned char>(c)) != 0)
    {
      if (!prevSpace)
      {
        out.push_back(' ');
      }
      prevSpace = true;
    }
    else
    {
      out.push_back(c);
      prevSpace = false;
    }
  }
  while (!out.empty() && out.back() == ' ')
  {
    out.pop_back();
  }
  return out;
}

std::vector<std::string> getSourceLines(const std::string &source)
{
  std::vector<std::string> lines;
  std::istringstream iss(source);
  std::string line;
  while (std::getline(iss, line))
  {
    lines.push_back(line);
  }
  return lines;
}

struct CollectContext
{
  const std::vector<Token> &tokens;
  const std::vector<int> &match;
  const FragmentOptions &opts;
  const std::string &file;
  const std::vector<std::string> &lines;
  std::vector<Fragment> &out;
};

// Collect the fragment's verbatim line views: a set for the union ratio, and an
// ordered sequence of substantive lines (drop "}", "{", "});") for the line-LCS run.
// A switch-skeleton line ("case X:", "break;", "default:", "switch (...)") carries no
// distinctive content — every switch has them. Counting them in the ordered line-run or
// classic line-overlap lets unrelated switches match on dispatch boilerplate.
bool isSwitchSkeletonLine(const std::string &n)
{
  if (n == "break;" || n == "default:" || n.rfind("switch (", 0) == 0 || n.rfind("switch(", 0) == 0)
  {
    return true;
  }
  return n.rfind("case ", 0) == 0 && n.back() == ':'; // bare case label, no inline body
}

bool isInlineSwitchTableLine(const std::string &n)
{
  if (n.rfind("case ", 0) != 0 && n.rfind("default:", 0) != 0)
  {
    return false;
  }
  const std::size_t colon = n.find(':');
  if (colon == std::string::npos)
  {
    return false;
  }
  return n.find(" break;", colon) != std::string::npos || n.find("; break;", colon) != std::string::npos;
}

void collectNormLines(Fragment &f, const std::vector<std::string> &lines)
{
  for (int ln = f.startLine; ln <= f.endLine; ++ln)
  {
    if (ln < 1 || static_cast<std::size_t>(ln) > lines.size())
    {
      continue;
    }
    std::string norm = normalizeLine(lines[ln - 1]);
    if (norm.empty())
    {
      continue;
    }
    if (isSwitchSkeletonLine(norm) || isInlineSwitchTableLine(norm))
    {
      continue;
    }
    if (norm.size() >= 4)
    {
      f.normLineSeq.push_back(norm);
    }
    f.normLines.insert(std::move(norm));
  }
}

// Tracks paren depth across `t[lo, hi)` and counts top-level `;` as statements
// (depth 0 excludes a for-header's own `;`s). Split out of makeFragment to keep
// the caller under the function-length threshold.
void trackParenAndStatements(const std::vector<Token> &t, std::size_t lo, std::size_t hi, int &paren, Fragment &f)
{
  for (std::size_t i = lo; i < hi; ++i)
  {
    const std::string &sym = t[i].sym;
    if (sym == "(")
    {
      ++paren;
    }
    else if (sym == ")")
    {
      if (paren > 0)
      {
        --paren;
      }
    }
    else if (sym == ";" && paren == 0)
    {
      ++f.statementCount;
    }
    ++f.bag[sym];
    f.seq.push_back(sym);
    f.rawSeq.push_back(t[i].raw.empty() ? sym : t[i].raw);
  }
}

Fragment makeFragment(const std::vector<Token> &t, std::size_t lo, std::size_t hi, const std::string &file,
                      const std::vector<std::string> &lines)
{
  Fragment f;
  f.file = file;
  f.startLine = t[lo].line;
  f.endLine = t[hi - 1].line;
  f.tokenCount = hi - lo;
  f.seq.reserve(hi - lo);
  f.rawSeq.reserve(hi - lo);
  int paren = 0;
  trackParenAndStatements(t, lo, hi, paren, f);
  collectNormLines(f, lines);
  f.diversity = trigramDiversity(f.seq);
  return f;
}

void pushNestedBoundary(const CollectContext &ctx, std::size_t lo, std::size_t hi)
{
  Fragment f = makeFragment(ctx.tokens, lo, hi, ctx.file, ctx.lines);
  f.nested = true;
  f.boundary = true;
  ctx.out.push_back(std::move(f));
}

void emitBoundaryRuns(const CollectContext &ctx, std::size_t bodyLo, std::size_t bodyHi)
{
  if (!ctx.opts.boundaryRuns)
  {
    return;
  }

  const std::array<std::size_t, 3> sizes = {ctx.opts.minTokens, ctx.opts.minTokens * 2, ctx.opts.minTokens * 4};
  for (std::size_t size : sizes)
  {
    if (bodyHi - bodyLo < size + ctx.opts.minTokens)
    {
      continue;
    }
    pushNestedBoundary(ctx, bodyLo, bodyLo + size);
    pushNestedBoundary(ctx, bodyHi - size, bodyHi);
  }
}

// Record `p` as a statement-start boundary if it opens a real statement (in range,
// not a bare `;`/`}` continuation).
void pushBoundary(std::vector<std::size_t> &starts, const CollectContext &ctx, std::size_t p, std::size_t hi)
{
  if (p < hi && ctx.tokens[p].sym != ";")
  {
    starts.push_back(p);
  }
}

// Top-level statement-start offsets within [lo, hi): `lo`, plus the index right
// after each top-level `;` (paren depth 0) and each matched-brace block. A matched
// `{...}` is skipped as a unit — a statement boundary never falls inside a brace
// group (a nested block, or a `cpr::Header h{...};` initializer) — so a window can
// never start mid-block or mid-expression. This is the interior generalization of
// #195's two fixed anchors (bodyLo / bodyHi) to every offset in between.
std::vector<std::size_t> statementStarts(const CollectContext &ctx, std::size_t lo, std::size_t hi)
{
  std::vector<std::size_t> starts{lo};
  int paren = 0;
  for (std::size_t i = lo; i < hi; ++i)
  {
    const std::string &sym = ctx.tokens[i].sym;
    if (sym == "(")
    {
      ++paren;
    }
    else if (sym == ")" && paren > 0)
    {
      --paren;
    }
    else if (sym == "{" && ctx.match[i] >= 0 && static_cast<std::size_t>(ctx.match[i]) < hi)
    {
      i = static_cast<std::size_t>(ctx.match[i]); // skip the whole block; ++i steps past `}`
      pushBoundary(starts, ctx, i + 1, hi);
    }
    else if (sym == ";" && paren == 0)
    {
      pushBoundary(starts, ctx, i + 1, hi);
    }
  }
  return starts;
}

// Smallest statement-boundary end at or beyond `s + tier` tokens (bodyHi if none).
std::size_t snapEnd(const std::vector<std::size_t> &starts, std::size_t si, std::size_t s, std::size_t tier,
                    std::size_t bodyHi)
{
  for (std::size_t sj = si + 1; sj < starts.size(); ++sj)
  {
    if (starts[sj] - s >= tier)
    {
      return starts[sj];
    }
  }
  return bodyHi;
}

// #191: emit bounded runs of top-level statements starting at *every* statement
// boundary of a body. For each start, snap the end to a statement boundary at ~min,
// 2*min and 4*min tokens (deduping equal snaps). Marked nested+boundary so the spans
// stay out of df/IDF (like #190/#195) yet reach the boundary-aware plain-Jaccard
// fallback when IDF has no signal.
void emitStatementRuns(const CollectContext &ctx, std::size_t bodyLo, std::size_t bodyHi)
{
  if (!ctx.opts.statementRuns || bodyHi <= bodyLo || bodyHi - bodyLo < ctx.opts.minTokens)
  {
    return;
  }
  const std::vector<std::size_t> starts = statementStarts(ctx, bodyLo, bodyHi);
  const std::array<std::size_t, 3> tiers = {ctx.opts.minTokens, ctx.opts.minTokens * 2, ctx.opts.minTokens * 4};
  for (std::size_t si = 0; si < starts.size(); ++si)
  {
    const std::size_t s = starts[si];
    std::size_t prevEnd = 0;
    for (std::size_t tier : tiers)
    {
      const std::size_t e = snapEnd(starts, si, s, tier, bodyHi);
      if (e - s >= ctx.opts.minTokens && e != prevEnd)
      {
        pushNestedBoundary(ctx, s, e);
        prevEnd = e;
      }
    }
  }
}

// True when the body opened at `brace` belongs to a control statement (if/for/while/
// switch/catch) rather than a function. Statement runs over control bodies are a pure
// FP flood — a switch's near-identical type-dispatch cases (`Select<int8_t,...>`,
// `Select<int16_t,...>`, …) get sliced into a clique of mutual RENAMED clones — so
// #191 slices only real function bodies. Walks back from the `)` before `{` to its
// matching `(` and inspects the preceding keyword.
bool isControlBody(const std::vector<Token> &t, std::size_t brace)
{
  if (brace == 0 || t[brace - 1].sym != ")")
  {
    return false;
  }
  int depth = 0;
  for (std::size_t k = brace; k-- > 0;)
  {
    if (t[k].sym == ")")
    {
      ++depth;
    }
    else if (t[k].sym == "(" && --depth == 0)
    {
      static const std::unordered_set<std::string> kw = {"if", "for", "while", "switch", "catch"};
      return k > 0 && kw.count(t[k - 1].sym) != 0;
    }
  }
  return false;
}

struct Range
{
  std::size_t from = 0;
  std::size_t to = 0;
  bool inside = false; // already within an emitted body -> anything emitted here is nested
};

void emitBodyAndDescend(const CollectContext &ctx, const Range &body, const Range &range, std::vector<Range> &work)
{
  Fragment f = makeFragment(ctx.tokens, body.from + 1, body.to, ctx.file, ctx.lines);
  f.nested = range.inside;
  ctx.out.push_back(std::move(f));
  emitBoundaryRuns(ctx, body.from + 1, body.to);
  work.push_back({body.to + 1, range.to, range.inside});
  work.push_back({body.from + 1, body.to, true});
}

// Scan [from, to): emit fn-body fragments inline; on the first nested block to
// descend, queue its inner range and the continuation on `work` and stop.
void scanRange(const CollectContext &ctx, const Range &range, std::vector<Range> &work)
{
  const std::size_t from = range.from;
  const std::size_t to = range.to;
  for (std::size_t i = from; i < to;)
  {
    const bool open = ctx.tokens[i].sym == "{" && ctx.match[i] >= 0 && static_cast<std::size_t>(ctx.match[i]) < to;
    if (!open)
    {
      ++i;
      continue;
    }
    const std::size_t j = static_cast<std::size_t>(ctx.match[i]);
    const std::size_t body = j - i - 1;
    const bool fnBody = (i > 0 && ctx.tokens[i - 1].sym == ")");
    if (fnBody && body > ctx.opts.maxTokens && !isControlBody(ctx.tokens, i))
    {
      // #191: statement runs over the interior of a body too large to be emitted
      // whole (emitBodyAndDescend below never fires for it, and #195's boundary
      // windows anchor to the ends), so a copied run offset in the MIDDLE of two
      // such functions still becomes a comparable fragment. Bodies within
      // [minTokens, maxTokens] are already covered by the whole body (#190) plus
      // its prefix/suffix boundary runs (#195); adding interior runs there is the
      // dominant idiom-flood / all-pairs cost source for no recall the others miss.
      emitStatementRuns(ctx, i + 1, j);
    }
    if (fnBody && body >= ctx.opts.minTokens && body <= ctx.opts.maxTokens)
    {
      // Emit the body, then descend into it anyway (#190): copied nested blocks
      // must be compared as their own fragments.
      emitBodyAndDescend(ctx, {i, j, false}, range, work);
      return;
    }
    // Descend into [i+1, j), then resume at j+1. Push continuation first so the
    // inner range pops (runs) first — matching the old recursion's pre-order.
    work.push_back({j + 1, to, range.inside});
    work.push_back({i + 1, j, range.inside});
    return;
  }
}

// Iterative DFS over brace-nested ranges. Recursion here was bounded only by the
// input's brace-nesting depth, so a pathologically deep file (e.g. ctags' vendored
// LLVM parser_overflow fixture — 16k nested braces) blew the call stack → SIGSEGV
// (only under -O2, where makeFragment inlines into the recursive frame). The explicit
// work stack keeps the original pre-order, so the emitted fragment set is identical.
void collect(const CollectContext &ctx, std::size_t lo, std::size_t hi)
{
  std::vector<Range> work;
  work.push_back({lo, hi, false});
  while (!work.empty())
  {
    const Range range = work.back();
    work.pop_back();
    scanRange(ctx, range, work);
  }
}

} // namespace

std::vector<Fragment> extractFragments(const std::vector<Token> &tokens, const std::string &source,
                                       const std::string &file, const FragmentOptions &opts)
{
  if (tokens.empty())
  {
    return {};
  }

  const std::vector<std::string> lines = getSourceLines(source);
  std::vector<Fragment> fragments;
  const std::vector<int> match = braceMatch(tokens);

  CollectContext ctx = {tokens, match, opts, file, lines, fragments};
  collect(ctx, 0, tokens.size());
  return fragments;
}

} // namespace archcheck::scan::duplication
