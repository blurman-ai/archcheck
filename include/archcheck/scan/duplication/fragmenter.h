#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "archcheck/scan/duplication/token_normalizer.h"

namespace archcheck::scan::duplication
{

struct Fragment
{
  std::string file;
  int startLine = 0;
  int endLine = 0;
  std::size_t tokenCount = 0;
  std::size_t statementCount = 0;            // top-level `;` (paren-depth 0): style-robust substance
  std::unordered_map<std::string, int> bag;  // normalized token -> count
  std::vector<std::string> seq;              // ordered normalized tokens (for LCS)
  std::vector<std::string> rawSeq;           // raw spelling per token, aligned with seq
  std::unordered_set<std::string> normLines; // line-set (Jaccard ratio)
  std::vector<std::string> normLineSeq;      // ordered substantive lines (line-LCS run)
  double diversity = 1.0;                    // distinct-trigram ratio (low = skeletal)
  // #190: this fragment is a block nested inside another emitted fragment. Such a
  // fragment is NOT a separate document: its tokens are already counted in the
  // enclosing body, so including it in document frequency would count them twice and
  // collapse IDF (an identical nested block scored ~0). Excluded from df/idf, which
  // also keeps the IDF distribution identical to pre-#190 — the guard thresholds are
  // calibrated against it.
  bool nested = false;
  bool boundary = false; // #195: synthetic function prefix/suffix span
};

struct FragmentOptions
{
  std::size_t minTokens = 30;
  // 600 not 400 (#091): twin functions that straddle the cap fragment
  // differently — one stays whole, the other subdivides — so the clone never
  // aligns. 600 keeps ~120-line functions whole, recovering function-level
  // copy-paste (LibreSprite algo_line/algo_line_float) without an FP blow-up.
  std::size_t maxTokens = 600;
  // #195: emit bounded function prefix/suffix spans so a shared prologue or
  // epilogue in two otherwise-different functions can be compared as a unit.
  bool boundaryRuns = false;
};

// Extract function-scale fragments from tokenized source.
// A "{" preceded by ")" is a function/control body; we emit such blocks if
// they fall within [minTokens, maxTokens]. Otherwise we descend to find
// inner bodies recursively. The source text is needed to extract source lines.
std::vector<Fragment> extractFragments(const std::vector<Token> &tokens, const std::string &source,
                                       const std::string &file, const FragmentOptions &opts = FragmentOptions{});

} // namespace archcheck::scan::duplication
