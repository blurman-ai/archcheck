#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

#include "archcheck/graph/dependency_graph.h"
#include "archcheck/graph/diff.h"

namespace archcheck::diff
{

struct AddedEdge
{
  std::string from;
  std::string to;
};

struct RemovedEdge
{
  std::string from;
  std::string to;
};

struct GrownCycle
{
  std::vector<std::string> members;
  std::size_t baselineSize = 0;
  std::size_t currentSize = 0;
};

// Aggregate result of comparing two dependency graphs. All NodeIds are
// resolved to paths up-front so the report stays valid after the source
// graphs go out of scope.
struct RegressionReport
{
  std::vector<AddedEdge> addedEdges;
  std::vector<RemovedEdge> removedEdges;
  std::vector<GrownCycle> grownCycles;

  bool hasRegression() const { return !addedEdges.empty() || !grownCycles.empty(); }
};

RegressionReport buildRegressionReport(const graph::DependencyGraph &baseline, const graph::DependencyGraph &current);

void writeTextReport(const RegressionReport &r, std::ostream &out);

} // namespace archcheck::diff
