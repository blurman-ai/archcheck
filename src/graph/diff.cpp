#include "archcheck/graph/diff.h"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "archcheck/graph/algorithms.h"
#include "archcheck/graph/dependency_graph.h"
#include "archcheck/graph/node_id.h"

namespace archcheck::graph
{

namespace
{

std::string_view canonicalPath(std::string_view path, const RenameMap &renames)
{
  if (renames.empty())
    return path;
  const auto it = renames.find(std::string(path));
  return it == renames.end() ? path : std::string_view{it->second};
}

std::unordered_map<std::string, NodeId> index_by_path(const DependencyGraph &g, const RenameMap &renames = {})
{
  std::unordered_map<std::string, NodeId> by_path;
  const std::size_t n = g.nodeCount();
  by_path.reserve(n);
  for (std::uint32_t i = 0; i < n; ++i)
  {
    by_path.emplace(std::string(canonicalPath(g.pathOf(NodeId{i}), renames)), NodeId{i});
  }
  return by_path;
}

std::optional<NodeId> lookup(const std::unordered_map<std::string, NodeId> &by_path, std::string_view path)
{
  auto it = by_path.find(std::string(path));
  if (it == by_path.end())
  {
    return std::nullopt;
  }
  return it->second;
}

void sort_edges(std::vector<EdgeRef> &edges)
{
  std::sort(edges.begin(), edges.end(), [](EdgeRef a, EdgeRef b)
            { return a.from.value < b.from.value || (a.from.value == b.from.value && a.to.value < b.to.value); });
}

std::vector<std::unordered_set<std::string>> nontrivial_scc_path_sets(const DependencyGraph &g,
                                                                      const std::vector<std::vector<NodeId>> &sccs,
                                                                      const RenameMap &renames = {})
{
  std::vector<std::unordered_set<std::string>> sets;
  sets.reserve(sccs.size());
  for (const auto &scc : sccs)
  {
    std::unordered_set<std::string> paths;
    if (scc.size() >= 2)
    {
      paths.reserve(scc.size());
      for (NodeId id : scc)
        paths.emplace(canonicalPath(g.pathOf(id), renames));
    }
    sets.push_back(std::move(paths));
  }
  return sets;
}

bool isSubsetOfAny(const std::unordered_set<std::string> &current_paths,
                   const std::vector<std::unordered_set<std::string>> &baseline_sets)
{
  for (const auto &b : baseline_sets)
  {
    if (b.size() < current_paths.size())
      continue;
    const bool allIn =
        std::all_of(current_paths.begin(), current_paths.end(), [&](const std::string &p) { return b.count(p) != 0; });
    if (allIn)
      return true;
  }
  return false;
}

std::size_t best_baseline_match(const std::unordered_set<std::string> &current_paths,
                                const std::vector<std::unordered_set<std::string>> &baseline_sets)
{
  std::size_t best_index = baseline_sets.size();
  std::size_t best_overlap = 0;
  for (std::size_t i = 0; i < baseline_sets.size(); ++i)
  {
    std::size_t overlap = 0;
    for (const auto &p : current_paths)
    {
      if (baseline_sets[i].count(p) != 0)
        ++overlap;
    }
    if (overlap > best_overlap)
    {
      best_overlap = overlap;
      best_index = i;
    }
  }
  return best_overlap == 0 ? baseline_sets.size() : best_index;
}

} // namespace

std::vector<EdgeRef> addedEdges(const DependencyGraph &baseline, const DependencyGraph &current,
                                const RenameMap &renames)
{
  const auto baseline_by_path = index_by_path(baseline, renames);
  std::vector<EdgeRef> result;
  const std::size_t n = current.nodeCount();
  for (std::uint32_t i = 0; i < n; ++i)
  {
    const NodeId from{i};
    const auto baseline_from = lookup(baseline_by_path, current.pathOf(from));
    for (NodeId to : current.successors(from))
    {
      const auto baseline_to = lookup(baseline_by_path, current.pathOf(to));
      if (!baseline_from || !baseline_to || !baseline.hasEdge(*baseline_from, *baseline_to))
        result.push_back(EdgeRef{from, to});
    }
  }
  sort_edges(result);
  return result;
}

std::vector<EdgeRef> removedEdges(const DependencyGraph &baseline, const DependencyGraph &current,
                                  const RenameMap &renames)
{
  const auto current_by_path = index_by_path(current);
  std::vector<EdgeRef> result;
  const std::size_t n = baseline.nodeCount();
  for (std::uint32_t i = 0; i < n; ++i)
  {
    const NodeId b_from{i};
    const auto current_from = lookup(current_by_path, canonicalPath(baseline.pathOf(b_from), renames));
    if (!current_from)
      continue;
    for (NodeId b_to : baseline.successors(b_from))
    {
      const auto current_to = lookup(current_by_path, canonicalPath(baseline.pathOf(b_to), renames));
      if (current_to && !current.hasEdge(*current_from, *current_to))
        result.push_back(EdgeRef{*current_from, *current_to});
    }
  }
  sort_edges(result);
  return result;
}

std::vector<GrownScc> grownSccs(const DependencyGraph &baseline, const DependencyGraph &current,
                                const RenameMap &renames)
{
  const auto baseline_sccs = computeScc(baseline);
  const auto current_sccs = computeScc(current);
  const auto baseline_sets = nontrivial_scc_path_sets(baseline, baseline_sccs, renames);
  const auto current_sets = nontrivial_scc_path_sets(current, current_sccs);
  std::vector<GrownScc> result;
  for (std::size_t i = 0; i < current_sccs.size(); ++i)
  {
    if (current_sccs[i].size() < 2 || isSubsetOfAny(current_sets[i], baseline_sets))
      continue;
    const std::size_t match = best_baseline_match(current_sets[i], baseline_sets);
    const std::size_t baselineSize = match == baseline_sets.size() ? 0 : baseline_sccs[match].size();
    result.push_back(GrownScc{current_sccs[i], baselineSize, current_sccs[i].size()});
  }
  return result;
}

} // namespace archcheck::graph
