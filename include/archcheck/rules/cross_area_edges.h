#pragma once

#include <cstdint>
#include <string>

#include "archcheck/graph/dependency_graph.h"
#include "archcheck/graph/node_id.h"
#include "archcheck/rules/area_of.h"

namespace archcheck::rules
{

// Visit every directed graph edge whose endpoints resolve to two DIFFERENT
// non-empty module areas, invoking fn(a, b) with the source/target module names.
// Parallel file-edges between the same module pair yield repeated (a, b) calls;
// callers that need uniqueness dedup as they consume.
template <typename Fn> void forEachCrossAreaEdge(const graph::DependencyGraph &g, Fn fn)
{
  for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(g.nodeCount()); ++i)
  {
    const graph::NodeId u{i};
    const std::string a = areaOf(g.pathOf(u));
    if (a.empty())
      continue;
    for (const auto v : g.successors(u))
    {
      const std::string b = areaOf(g.pathOf(v));
      if (!b.empty() && a != b)
        fn(a, b);
    }
  }
}

} // namespace archcheck::rules
