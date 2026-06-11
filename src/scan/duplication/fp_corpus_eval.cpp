#include "archcheck/scan/duplication/fp_corpus_eval.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace archcheck::scan::duplication
{

// Simple TSV parser: repo | sha | label | fp_class | klass | added_loc | base_loc | note
GroundTruth loadCorpusGroundTruth(const std::string &corpusPath)
{
  GroundTruth truth;
  std::ifstream file(corpusPath);

  if (!file.is_open())
  {
    return truth; // Empty on error
  }

  std::string line;
  while (std::getline(file, line))
  {
    if (line.empty() || line[0] == '#')
      continue; // Skip comments/empty

    std::istringstream iss(line);
    std::string repo, sha, label, fpClass, klass;

    if (!(iss >> repo >> sha >> label >> fpClass >> klass))
      continue;

    // Key: repo:sha for simplicity (full ground truth would use line numbers)
    std::string key = repo + ":" + sha;
    truth[key] = {label, fpClass}; // {TP/FP, class name}
  }

  return truth;
}

} // namespace archcheck::scan::duplication
