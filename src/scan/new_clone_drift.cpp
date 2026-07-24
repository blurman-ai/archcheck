#include "archcheck/scan/new_clone_drift.h"

#include <functional>
#include <string>
#include <unordered_set>

#include "archcheck/scan/duplication/duplication_scanner.h"
#include "archcheck/scan/source_snapshot.h"

namespace archcheck::scan
{

namespace
{

constexpr const char *kRuleId = "DRIFT.NEW_CLONE";

// Normalized tokens of a fragment joined by a unit separator (the shared body of
// both key functions below).
std::string joinTokens(const duplication::Fragment &f)
{
  std::string joined;
  for (const auto &t : f.seq)
  {
    joined += t;
    joined.push_back('\x1f');
  }
  return joined;
}

// Location-aware identity of a fragment: its file plus the hash of its normalized
// token sequence. Including the file is what lets the parent-guard tell apart "this
// PR reformatted a copy that already existed here" (same file+content → suppress)
// from "this PR pasted a new copy into a new location" (new file → a real new clone,
// even if the same content was already duplicated elsewhere). The token hash stays
// stable across reformat/whitespace churn, so the reformat case is still recognised.
std::string fragKey(const duplication::Fragment &f)
{
  return std::to_string(std::hash<std::string>{}(f.file + '\x1f' + joinTokens(f)));
}

std::string contentKey(const duplication::Fragment &f)
{
  return std::to_string(std::hash<std::string>{}(joinTokens(f)));
}

// Order-independent key for a clone pair, so parent and new trees match regardless
// of which side the scanner happened to label `a`.
std::string pairKey(const duplication::Fragment &a, const duplication::Fragment &b)
{
  const auto ka = fragKey(a);
  const auto kb = fragKey(b);
  return ka < kb ? ka + "|" + kb : kb + "|" + ka;
}

// Clone pairs that already exist in the parent tree, keyed by content. A pair in
// this set was not introduced by the diff, even if the diff touched one side.
std::unordered_set<std::string> parentPairKeys(const duplication::ScanResult &scan)
{
  std::unordered_set<std::string> keys;
  for (const auto &p : scan.pairs)
    keys.insert(pairKey(scan.fragments[p.a], scan.fragments[p.b]));
  return keys;
}

// True if any line of [f.startLine, f.endLine] appears in lineMap[f.file].
// AddedLineMap and DeletedLineMap are the same type, so one query serves both the
// added-line and deleted-line lookups (caller names the direction via the argument).
bool fragmentTouches(const duplication::Fragment &f, const AddedLineMap &lineMap)
{
  const auto it = lineMap.find(f.file);
  if (it == lineMap.end())
    return false;
  for (int ln = f.startLine; ln <= f.endLine; ++ln)
    if (it->second.count(ln))
      return true;
  return false;
}

std::unordered_set<std::string> deletedFragmentKeys(const duplication::ScanResult &scan, const DeletedLineMap &deleted)
{
  std::unordered_set<std::string> keys;
  if (deleted.empty())
    return keys;
  for (const auto &f : scan.fragments)
    if (fragmentTouches(f, deleted))
      keys.insert(contentKey(f));
  return keys;
}

rules::Violation makeViolation(const duplication::Fragment &introduced, const duplication::Fragment &source,
                               const duplication::Pair &p)
{
  rules::Violation v;
  v.ruleId = kRuleId;
  v.file = introduced.file;
  v.line = introduced.startLine;
  // Show BOTH spans so the introduced block and its source line up — a single
  // introduced line next to a source range reads as a mismatch (it isn't).
  v.message = "copy-paste introduced (" + p.type + "): lines " + std::to_string(introduced.startLine) + "-" +
              std::to_string(introduced.endLine) + " - clone of " + source.file + ":" +
              std::to_string(source.startLine) + "-" + std::to_string(source.endLine);
  return v;
}

// #149: total authored bytes — the whole-tree clone scan cost scales with this.
std::size_t authoredBytes(const std::vector<std::pair<std::string, std::string>> &sources)
{
  std::size_t bytes = 0;
  for (const auto &s : sources)
    bytes += s.second.size();
  return bytes;
}

// Scan the new tree and emit one violation per introduced clone pair: a pair where
// one side touches an added line and the parent did not already contain it.
NewCloneDriftResult emitNewClones(const std::vector<std::pair<std::string, std::string>> &sources,
                                  const std::unordered_set<std::string> &parentKeys,
                                  const std::unordered_set<std::string> &deletedKeys, const AddedLineMap &added,
                                  const std::unordered_set<std::string> &focus)
{
  NewCloneDriftResult result;
  // Whole-file guard ON (#179 follow-up): a MULTI-fragment whole-file / whole-subtree
  // twin (a base-lib mounted under a new prefix, a file re-rooted, a vendored byte-copy)
  // is move/copy noise even when a commit adds it — P0.2's two-sided isWholeFileClone
  // (lo>=2, matched*5>=hi*4) cuts it while keeping a single-fragment copy and an
  // extraction (small⊂large) reported. Makes --diff agree with --duplication, which
  // already refuses to flag such twins. Precision filters (joint floor, P1) stay on.
  duplication::ScannerOptions opts;
  opts.enableWholeFileGuard = true;
  opts.focusFiles = focus;
  const auto scan = duplication::scanForDuplication(sources, opts);
  for (const auto &p : scan.pairs)
  {
    const auto &fa = scan.fragments[p.a];
    const auto &fb = scan.fragments[p.b];
    const bool aTouched = fragmentTouches(fa, added);
    const bool bTouched = fragmentTouches(fb, added);
    if (!aTouched && !bTouched)
      continue;
    if ((aTouched && deletedKeys.count(contentKey(fa)) != 0) || (bTouched && deletedKeys.count(contentKey(fb)) != 0))
      continue; // moved from deleted parent lines, not an additional copy
    if (parentKeys.count(pairKey(fa, fb)))
      continue; // pre-existing clone, diff merely touched it
    result.violations.push_back(aTouched ? makeViolation(fa, fb, p) : makeViolation(fb, fa, p));
  }
  return result;
}

} // namespace

NewCloneDriftResult detectNewClones(const SourceSnapshot &newSnapshot, const SourceSnapshot &parentSnapshot,
                                    const AddedLineMap &added, const DeletedLineMap &deleted, std::size_t maxScanBytes)
{
  NewCloneDriftResult result;
  if (added.empty())
    return result;
  const auto sources = newSnapshot.authoredSources();
  // #149: the clone scan is a whole-tree pass twice over (parent + new); on a huge
  // authored tree it blows the per-commit budget. Skip past the cap — advisory only,
  // so the gate (cycles/god-headers) still runs and the commit is not lost.
  if (authoredBytes(sources) > maxScanBytes)
  {
    result.skippedLargeTree = true;
    return result;
  }
  // #180: only pairs incident to a changed file can be a NEW clone, so scan with the commit's
  // touched files as the focus set — cuts the all-pairs scoring on large trees, same result.
  std::unordered_set<std::string> focus;
  for (const auto &[file, lines] : added)
    focus.insert(file);
  for (const auto &[file, lines] : deleted)
    focus.insert(file);
  // One parent scan feeds both the pre-existing-pair guard and the deleted-fragment guard
  // (was scanned twice — the whole-tree pass is the per-commit cost, #180).
  duplication::ScannerOptions parentOpts;
  parentOpts.enableWholeFileGuard = false;
  parentOpts.focusFiles = focus;
  const auto parentScan = duplication::scanForDuplication(parentSnapshot.authoredSources(), parentOpts);
  return emitNewClones(sources, parentPairKeys(parentScan), deletedFragmentKeys(parentScan, deleted), added, focus);
}

NewCloneDriftResult detectNewClones(const SourceSnapshot &newSnapshot, const SourceSnapshot &parentSnapshot,
                                    const AddedLineMap &added, std::size_t maxScanBytes)
{
  return detectNewClones(newSnapshot, parentSnapshot, added, DeletedLineMap{}, maxScanBytes);
}

} // namespace archcheck::scan
