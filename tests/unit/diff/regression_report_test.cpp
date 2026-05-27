#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <sstream>

#include "archcheck/diff/regression_report.h"
#include "archcheck/graph/dependency_graph.h"

using archcheck::diff::buildRegressionReport;
using archcheck::diff::writeTextReport;
using archcheck::graph::DependencyGraph;
using archcheck::graph::NodeId;

namespace
{

DependencyGraph chain_abc()
{
  DependencyGraph g;
  const NodeId a = g.addNode("a.h");
  const NodeId b = g.addNode("b.h");
  const NodeId c = g.addNode("c.h");
  g.addEdge(a, b);
  g.addEdge(b, c);
  return g;
}

} // namespace

TEST_CASE("buildRegressionReport: no diff → empty, no regression", "[diff][report]")
{
  const auto baseline = chain_abc();
  const auto current = chain_abc();
  const auto r = buildRegressionReport(baseline, current);
  REQUIRE(r.addedEdges.empty());
  REQUIRE(r.removedEdges.empty());
  REQUIRE(r.grownCycles.empty());
  REQUIRE_FALSE(r.hasRegression());
}

TEST_CASE("buildRegressionReport: shortcut edge a->c is reported as added", "[diff][report]")
{
  const auto baseline = chain_abc();
  DependencyGraph current = chain_abc();
  current.addEdge(NodeId{0}, NodeId{2});

  const auto r = buildRegressionReport(baseline, current);
  REQUIRE(r.addedEdges.size() == 1);
  REQUIRE(r.addedEdges[0].from == "a.h");
  REQUIRE(r.addedEdges[0].to == "c.h");
  REQUIRE(r.hasRegression());
}

TEST_CASE("buildRegressionReport: new cycle is reported as grown SCC", "[diff][report]")
{
  const auto baseline = chain_abc();
  DependencyGraph current = chain_abc();
  current.addEdge(NodeId{2}, NodeId{0}); // closes a->b->c->a

  const auto r = buildRegressionReport(baseline, current);
  REQUIRE_FALSE(r.grownCycles.empty());
  REQUIRE(r.grownCycles[0].currentSize == 3);
  REQUIRE(r.grownCycles[0].baselineSize == 0);
  REQUIRE(r.hasRegression());
}

TEST_CASE("writeTextReport: removed-only diff is not a regression", "[diff][report]")
{
  // Removing edges should NOT trip exit-code-1 in CI — PRs that drop edges
  // make architecture stricter, not worse.
  DependencyGraph baseline = chain_abc();
  DependencyGraph current;
  current.addNode("a.h");
  current.addNode("b.h");
  current.addNode("c.h");
  // current has no edges at all.

  const auto r = buildRegressionReport(baseline, current);
  REQUIRE(r.addedEdges.empty());
  REQUIRE(r.removedEdges.size() == 2);
  REQUIRE_FALSE(r.hasRegression());

  std::ostringstream out;
  writeTextReport(r, out);
  const auto s = out.str();
  REQUIRE(s.find("added_edges:    0") != std::string::npos);
  REQUIRE(s.find("removed_edges:  2") != std::string::npos);
}
