#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "archcheck/graph/dependency_graph.h"
#include "archcheck/graph/node_id.h"

namespace archcheck::graph
{

// Maps baseline (old) repository-relative paths to current (new) paths for renames.
using RenameMap = std::unordered_map<std::string, std::string>;

struct EdgeRef
{
  NodeId from;
  NodeId to;
};

// Edges present in `current` but not in `baseline`. Both graphs are
// compared by path (string), not by NodeId — NodeIds are session-local.
// When `renames` is non-empty, baseline paths are canonicalized to their new paths
// before comparison so pure file moves report 0 added edges.
// Returned EdgeRef uses NodeIds from the `current` graph.
std::vector<EdgeRef> addedEdges(const DependencyGraph &baseline, const DependencyGraph &current,
                                const RenameMap &renames = {});

// Edges present in `baseline` but not in `current`. Returned EdgeRef uses
// NodeIds from the `current` graph; if an endpoint exists only in baseline,
// we skip such edges, because a removed edge whose endpoint also disappeared is
// redundant for drift reporting. Baseline paths are canonicalized via `renames`.
std::vector<EdgeRef> removedEdges(const DependencyGraph &baseline, const DependencyGraph &current,
                                  const RenameMap &renames = {});

struct GrownScc
{
  std::vector<NodeId> members; // NodeIds from `current` graph
  std::size_t baselineSize;
  std::size_t currentSize;
};

// SCCs in `current` of size >= 2 that are not contained in any single non-trivial
// SCC of `baseline` (after mapping baseline paths through `renames`). Catches new
// cycles, grown cycles, and equal-size membership replacements ({a,b} -> {a,c}).
std::vector<GrownScc> grownSccs(const DependencyGraph &baseline, const DependencyGraph &current,
                                const RenameMap &renames = {});

} // namespace archcheck::graph
