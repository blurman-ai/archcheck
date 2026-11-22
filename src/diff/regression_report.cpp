#include "archcheck/diff/regression_report.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include "archcheck/graph/algorithms.h"
#include "archcheck/graph/diff.h"
#include "archcheck/graph/node_id.h"

namespace archcheck::diff
{

namespace
{

struct AreaClassifier
{
  bool useSecondLevel = false;

  std::string classify(std::string_view path) const
  {
    const auto firstSlash = path.find('/');
    if (firstSlash == std::string_view::npos)
      return "<root>";
    if (!useSecondLevel)
      return std::string(path.substr(0, firstSlash));
    const auto secondSlash = path.find('/', firstSlash + 1);
    if (secondSlash == std::string_view::npos)
      return std::string(path.substr(0, firstSlash));
    return std::string(path.substr(0, secondSlash));
  }
};

struct AreaEdgeStats
{
  std::string fromArea;
  std::string toArea;
  std::size_t edgeCount = 0;
  std::string sampleFrom;
  std::string sampleTo;
};

using AreaEdgeMap = std::unordered_map<std::string, AreaEdgeStats>;

std::string_view canonicalPath(std::string_view path, const graph::RenameMap &renames)
{
  if (renames.empty())
    return path;
  const auto it = renames.find(std::string(path));
  return it == renames.end() ? path : std::string_view{it->second};
}

AreaClassifier buildAreaClassifier(const graph::DependencyGraph &baseline, const graph::DependencyGraph &current,
                                   const graph::RenameMap &renames)
{
  std::unordered_set<std::string> topDirs;
  const auto collectTopDirs = [&](const graph::DependencyGraph &g, const graph::RenameMap &rm)
  {
    for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(g.nodeCount()); ++i)
    {
      const auto path = canonicalPath(g.pathOf(graph::NodeId{i}), rm);
      const auto slash = path.find('/');
      if (slash != std::string_view::npos)
        topDirs.emplace(path.substr(0, slash));
    }
  };
  collectTopDirs(baseline, renames);
  collectTopDirs(current, {});
  return AreaClassifier{topDirs.size() == 1};
}

std::string makeAreaKey(std::string_view fromArea, std::string_view toArea)
{
  std::string key;
  key.reserve(fromArea.size() + toArea.size() + 1);
  key.append(fromArea);
  key.push_back('\n');
  key.append(toArea);
  return key;
}

AreaEdgeMap collectCrossAreaEdges(const graph::DependencyGraph &g, const AreaClassifier &classifier,
                                  const graph::RenameMap &renames = {})
{
  AreaEdgeMap result;
  for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(g.nodeCount()); ++i)
  {
    const graph::NodeId from{i};
    const auto fromPath = canonicalPath(g.pathOf(from), renames);
    const auto fromArea = classifier.classify(fromPath);
    for (graph::NodeId to : g.successors(from))
    {
      const auto toPath = canonicalPath(g.pathOf(to), renames);
      const auto toArea = classifier.classify(toPath);
      if (fromArea == toArea)
        continue;
      const auto key = makeAreaKey(fromArea, toArea);
      auto &stats = result[key];
      if (stats.edgeCount == 0)
      {
        stats.fromArea = fromArea;
        stats.toArea = toArea;
        stats.sampleFrom = std::string(fromPath);
        stats.sampleTo = std::string(toPath);
      }
      ++stats.edgeCount;
    }
  }
  return result;
}

std::vector<NewCrossAreaDependency> detectNewCrossAreaDependencies(const graph::DependencyGraph &baseline,
                                                                   const graph::DependencyGraph &current,
                                                                   const graph::RenameMap &renames)
{
  const auto classifier = buildAreaClassifier(baseline, current, renames);
  const auto baselineAreas = collectCrossAreaEdges(baseline, classifier, renames);
  const auto currentAreas = collectCrossAreaEdges(current, classifier);
  std::vector<NewCrossAreaDependency> result;
  for (const auto &[key, stats] : currentAreas)
  {
    if (baselineAreas.count(key) != 0)
      continue;
    result.push_back(
        NewCrossAreaDependency{stats.fromArea, stats.toArea, stats.edgeCount, stats.sampleFrom, stats.sampleTo});
  }
  std::sort(result.begin(), result.end(), [](const NewCrossAreaDependency &a, const NewCrossAreaDependency &b)
            { return a.fromArea < b.fromArea || (a.fromArea == b.fromArea && a.toArea < b.toArea); });
  return result;
}

} // namespace

void collectEdgesAndCycles(const graph::DependencyGraph &baseline, const graph::DependencyGraph &current,
                           RegressionReport &r, const graph::RenameMap &renames)
{
  for (const auto &e : graph::addedEdges(baseline, current, renames))
    r.addedEdges.push_back(
        {std::string{current.pathOf(e.from)}, std::string{current.pathOf(e.to)}}); // LCOV_EXCL_BR_LINE
  for (const auto &e : graph::removedEdges(baseline, current, renames))
    r.removedEdges.push_back(
        {std::string{current.pathOf(e.from)}, std::string{current.pathOf(e.to)}}); // LCOV_EXCL_BR_LINE
  for (const auto &g : graph::grownSccs(baseline, current, renames))
  {
    GrownCycle gc;
    gc.baselineSize = g.baselineSize;
    gc.currentSize = g.currentSize;
    gc.members.reserve(g.members.size()); // LCOV_EXCL_BR_LINE
    for (const auto &m : g.members)
      gc.members.emplace_back(current.pathOf(m)); // LCOV_EXCL_BR_LINE
    r.grownCycles.push_back(std::move(gc));       // LCOV_EXCL_BR_LINE
  }
}

std::vector<std::string> detectNewGodHeaders(const graph::DependencyGraph &baseline,
                                             const graph::DependencyGraph &current, std::uint32_t fanInThreshold,
                                             const graph::RenameMap &renames)
{
  const auto bFanIn = graph::computeFanIn(baseline);
  const auto cFanIn = graph::computeFanIn(current);
  std::unordered_set<std::string> baselineGodSet;
  for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(baseline.nodeCount()); ++i)
    if (bFanIn[i] > fanInThreshold)
      baselineGodSet.emplace(canonicalPath(baseline.pathOf(graph::NodeId{i}), renames));
  std::vector<std::string> result;
  for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(current.nodeCount()); ++i)
    if (cFanIn[i] > fanInThreshold)
    {
      auto name = std::string{current.pathOf(graph::NodeId{i})};
      if (!baselineGodSet.count(name))
        result.push_back(std::move(name));
    }
  return result;
}

RegressionReport buildRegressionReport(const graph::DependencyGraph &baseline, const graph::DependencyGraph &current,
                                       MetricThresholds thresholds, const graph::RenameMap &renames)
{
  RegressionReport r;
  collectEdgesAndCycles(baseline, current, r, renames);
  r.newCrossAreaDependencies = detectNewCrossAreaDependencies(baseline, current, renames);
  const auto bm = graph::computeGraphMetrics(baseline);
  const auto cm = graph::computeGraphMetrics(current);
  if (cm.maxChainLength > bm.maxChainLength)
    r.chainLengthGrown = MetricDelta{bm.maxChainLength, cm.maxChainLength};
  r.newGodHeaders = detectNewGodHeaders(baseline, current, thresholds.godHeaderFanIn, renames);
  if (cm.nccd > bm.nccd)
    r.nccdDelta = cm.nccd - bm.nccd;
  return r;
}

void writeAdded(const std::vector<AddedEdge> &v, std::ostream &out)
{
  if (v.empty())
    return;
  out << "\nadded (advisory):\n"; // LCOV_EXCL_BR_LINE
  for (const auto &e : v)
    out << "  + " << e.from << "  ->  " << e.to << '\n'; // LCOV_EXCL_BR_LINE
}

void writeRemoved(const std::vector<RemovedEdge> &v, std::ostream &out)
{
  if (v.empty())
    return;
  out << "\nremoved (advisory):\n"; // LCOV_EXCL_BR_LINE
  for (const auto &e : v)
    out << "  - " << e.from << "  ->  " << e.to << '\n'; // LCOV_EXCL_BR_LINE
}

void writeGrown(const std::vector<GrownCycle> &v, std::ostream &out)
{
  if (v.empty())
    return;
  out << "\ngrown_cycles (gating):\n"; // LCOV_EXCL_BR_LINE
  for (const auto &g : v)
  {
    out << "  * size " << g.baselineSize << " -> " << g.currentSize << " (" << g.members.size()
        << " members)\n"; // LCOV_EXCL_BR_LINE
    for (const auto &m : g.members)
      out << "      " << m << '\n'; // LCOV_EXCL_BR_LINE
  }
}

void writeCrossAreaDependencies(const std::vector<NewCrossAreaDependency> &v, std::ostream &out)
{
  if (v.empty())
    return;
  out << "\nnew_cross_area_dependencies (advisory):\n"; // LCOV_EXCL_BR_LINE
  for (const auto &dep : v)
  {
    out << "  * " << dep.fromArea << " -> " << dep.toArea << " (" << dep.edgeCount << " edge";
    if (dep.edgeCount != 1)
      out << 's';
    out << ")\n";                                                               // LCOV_EXCL_BR_LINE
    out << "      e.g. " << dep.sampleFrom << "  ->  " << dep.sampleTo << '\n'; // LCOV_EXCL_BR_LINE
  }
}

void writeChainLength(const std::optional<MetricDelta> &d, std::ostream &out)
{
  if (!d)
    return;
  out << "\nchain_length_grown (advisory): " << d->baseline << " -> " << d->current << '\n'; // LCOV_EXCL_BR_LINE
}

void writeGodHeaders(const std::vector<std::string> &v, std::ostream &out)
{
  if (v.empty())
    return;
  out << "\nnew_god_headers (gating):\n"; // LCOV_EXCL_BR_LINE
  for (const auto &h : v)
    out << "  ! " << h << '\n'; // LCOV_EXCL_BR_LINE
}

void writeNccdDelta(const std::optional<double> &d, std::ostream &out)
{
  if (!d)
    return;
  out << "\nnccd_grown (advisory): +" << *d << '\n'; // LCOV_EXCL_BR_LINE
}

void writeTextReport(const RegressionReport &r, std::ostream &out)
{
  out << "added_edges:    " << r.addedEdges.size() << '\n' // LCOV_EXCL_BR_LINE
      << "removed_edges:  " << r.removedEdges.size() << '\n'
      << "grown_cycles:   " << r.grownCycles.size() << '\n'
      << "new_area_deps:  " << r.newCrossAreaDependencies.size() << '\n';
  writeAdded(r.addedEdges, out);                               // LCOV_EXCL_BR_LINE
  writeRemoved(r.removedEdges, out);                           // LCOV_EXCL_BR_LINE
  writeGrown(r.grownCycles, out);                              // LCOV_EXCL_BR_LINE
  writeCrossAreaDependencies(r.newCrossAreaDependencies, out); // LCOV_EXCL_BR_LINE
  writeChainLength(r.chainLengthGrown, out);                   // LCOV_EXCL_BR_LINE
  writeGodHeaders(r.newGodHeaders, out);                       // LCOV_EXCL_BR_LINE
  writeNccdDelta(r.nccdDelta, out);                            // LCOV_EXCL_BR_LINE
  out << "\ngate: " << (r.gates() ? "fail" : "ok") << " (gating: cycles, god-headers)\n";
}

std::size_t dropRenameArtifactCycles(RegressionReport &r, const std::unordered_set<std::string> &renamedNewPaths)
{
  if (renamedNewPaths.empty())
    return 0;
  auto &v = r.grownCycles;
  const auto before = v.size();
  const auto allRenamed = [&](const GrownCycle &c)
  {
    return !c.members.empty() && std::all_of(c.members.begin(), c.members.end(),
                                             [&](const std::string &m) { return renamedNewPaths.count(m) != 0; });
  };
  v.erase(std::remove_if(v.begin(), v.end(), allRenamed), v.end());
  return before - v.size();
}

} // namespace archcheck::diff
