#include "archcheck/diff/regression_report.h"

#include <string>

#include "archcheck/graph/diff.h"

namespace archcheck::diff
{

RegressionReport buildRegressionReport(const graph::DependencyGraph &baseline, const graph::DependencyGraph &current)
{
  RegressionReport r;
  for (const auto &e : graph::addedEdges(baseline, current))
  {
    r.addedEdges.push_back({std::string{current.pathOf(e.from)}, std::string{current.pathOf(e.to)}});
  }
  for (const auto &e : graph::removedEdges(baseline, current))
  {
    r.removedEdges.push_back({std::string{current.pathOf(e.from)}, std::string{current.pathOf(e.to)}});
  }
  for (const auto &g : graph::grownSccs(baseline, current))
  {
    GrownCycle gc;
    gc.baselineSize = g.baselineSize;
    gc.currentSize = g.currentSize;
    gc.members.reserve(g.members.size());
    for (const auto &m : g.members)
    {
      gc.members.emplace_back(current.pathOf(m));
    }
    r.grownCycles.push_back(std::move(gc));
  }
  return r;
}

namespace
{

void writeAdded(const std::vector<AddedEdge> &v, std::ostream &out)
{
  if (v.empty())
    return;
  out << "\nadded:\n";
  for (const auto &e : v)
    out << "  + " << e.from << "  ->  " << e.to << '\n';
}

void writeRemoved(const std::vector<RemovedEdge> &v, std::ostream &out)
{
  if (v.empty())
    return;
  out << "\nremoved:\n";
  for (const auto &e : v)
    out << "  - " << e.from << "  ->  " << e.to << '\n';
}

void writeGrown(const std::vector<GrownCycle> &v, std::ostream &out)
{
  if (v.empty())
    return;
  out << "\ngrown_cycles:\n";
  for (const auto &g : v)
  {
    out << "  * size " << g.baselineSize << " -> " << g.currentSize << " (" << g.members.size() << " members)\n";
    for (const auto &m : g.members)
      out << "      " << m << '\n';
  }
}

} // namespace

void writeTextReport(const RegressionReport &r, std::ostream &out)
{
  out << "added_edges:    " << r.addedEdges.size() << '\n'
      << "removed_edges:  " << r.removedEdges.size() << '\n'
      << "grown_cycles:   " << r.grownCycles.size() << '\n';
  writeAdded(r.addedEdges, out);
  writeRemoved(r.removedEdges, out);
  writeGrown(r.grownCycles, out);
}

} // namespace archcheck::diff
