#pragma once

// Research / QA precision-evaluation harness for the duplication detector.
// NOT part of the shipped archcheck_core library: this translation unit is
// compiled into archcheck_tests only (see tests/CMakeLists.txt). It measures
// detector precision against a labelled corpus and is internal QA tooling, not
// a user-facing capability. `evaluateAgainstCorpus` is currently a placeholder
// (full content-hash matching is future work — see #083/#084).

#include <string>
#include <unordered_map>
#include <vector>

#include "archcheck/scan/duplication/duplication_scanner.h"

namespace archcheck::scan::duplication
{

// Load ground truth from experiments/verification/fp_corpus_r2.tsv.
// Returns map: (repo, sha, line_a, line_b) → label (TP or FP), class name.
// On parse error, returns empty map.
using GroundTruth = std::unordered_map<std::string, std::pair<std::string, std::string>>;
GroundTruth loadCorpusGroundTruth(const std::string &corpusPath);

} // namespace archcheck::scan::duplication
