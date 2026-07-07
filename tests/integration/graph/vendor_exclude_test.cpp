#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

#include "archcheck/graph/graph_builder.h"
#include "archcheck/scan/file_classification.h"

// #069/#068: vendored single-file libs and vendor directories must not enter the
// include graph — their cycles and fan-in are noise, not author drift.

using archcheck::graph::buildGraphForPath;

namespace
{

std::filesystem::path fixture(std::string_view name)
{
  return std::filesystem::path{ARCHCHECK_FIXTURES_DIR} / "vendored_version_dir" / name;
}

struct TempTree
{
  std::filesystem::path root;
  ~TempTree()
  {
    std::error_code ec;
    std::filesystem::remove_all(root, ec);
  }
};

TempTree make_tree()
{
  static std::atomic<int> counter{0};
  const auto base =
      std::filesystem::temp_directory_path() / ("archcheck_vendor_" + std::to_string(counter.fetch_add(1)));
  std::error_code ec;
  std::filesystem::remove_all(base, ec);
  std::filesystem::create_directories(base);
  return TempTree{base};
}

void write(const std::filesystem::path &p, const std::string &content)
{
  std::filesystem::create_directories(p.parent_path());
  std::ofstream{p} << content;
}

bool graph_contains(const archcheck::graph::DependencyGraph &g, std::string_view needle)
{
  for (std::uint32_t i = 0; i < g.nodeCount(); ++i)
  {
    if (g.pathOf(archcheck::graph::NodeId{i}).find(needle) != std::string_view::npos)
    {
      return true;
    }
  }
  return false;
}

} // namespace

TEST_CASE("graph excludes vendored single-file lib by curated name", "[graph][vendor]")
{
  const TempTree tree = make_tree();
  write(tree.root / "app.cpp", "#include \"util.h\"\n");
  write(tree.root / "util.h", "// author util\n");
  // qcustomplot.* is a curated single-file lib (layer 1) dropped straight into src.
  write(tree.root / "qcustomplot.h", "#include \"qcustomplot.cpp\"\n");
  write(tree.root / "qcustomplot.cpp", "#include \"qcustomplot.h\"\n"); // self-cycle

  const auto built = buildGraphForPath(tree.root);

  REQUIRE(graph_contains(built.graph, "app.cpp"));
  REQUIRE(graph_contains(built.graph, "util.h"));
  REQUIRE_FALSE(graph_contains(built.graph, "qcustomplot"));
  REQUIRE(built.graph.nodeCount() == 2);
}

TEST_CASE("graph excludes vendored lib by permissive-license header", "[graph][vendor]")
{
  const TempTree tree = make_tree();
  write(tree.root / "app.cpp", "// author code\n");
  // Renamed/unknown vendor caught only by the license banner (layer 2). Uses the
  // full verbatim MIT text — a bare SPDX tag is no longer a vendor signal (#081).
  write(tree.root / "renamed_lib.h", "// Permission is hereby granted, free of charge\nint vendored();\n");

  const auto built = buildGraphForPath(tree.root);

  REQUIRE(graph_contains(built.graph, "app.cpp"));
  REQUIRE_FALSE(graph_contains(built.graph, "renamed_lib"));
}

TEST_CASE("graph excludes vendored directory subtrees", "[graph][vendor]")
{
  const TempTree tree = make_tree();
  write(tree.root / "src" / "main.cpp", "// author\n");
  write(tree.root / "third_party" / "fmt" / "format.h", "// vendor\n");
  write(tree.root / "extern" / "sdl" / "sdl.h", "// vendor\n");

  const auto built = buildGraphForPath(tree.root);

  REQUIRE(graph_contains(built.graph, "src/main.cpp"));
  REQUIRE_FALSE(graph_contains(built.graph, "third_party"));
  REQUIRE_FALSE(graph_contains(built.graph, "extern"));
  REQUIRE(built.graph.nodeCount() == 1);
}

TEST_CASE("graph drops net-snmp version+qualifier dir, keeps authored lookalike (#179)", "[graph][vendor]")
{
  // ezsnmp vendors net-snmp as net-snmp-<ver>-final-patched/: a dotted version with an
  // alpha qualifier trailing it. The stem before the version (netsnmp) is a curated
  // vendored token, so the subtree drops; libfoo-2d-renderer has a digit tail too but
  // its stem is NOT a token, so the authored subtree stays. Fixtures on disk (#179).
  const auto pass = buildGraphForPath(fixture("pass"));
  REQUIRE(graph_contains(pass.graph, "libfoo-2d-renderer/foo.cpp"));

  const auto fail = buildGraphForPath(fixture("fail_netsnmp"));
  REQUIRE(graph_contains(fail.graph, "app.cpp"));        // authored contrast stays
  REQUIRE_FALSE(graph_contains(fail.graph, "net-snmp")); // versioned vendored subtree dropped
  REQUIRE(fail.graph.nodeCount() == 1);
}

TEST_CASE("graph keeps net-snmp version dir when it IS the scan root (self-project, #179)", "[graph][vendor]")
{
  // Running archcheck ON the net-snmp repo: the scan root's own name (net-snmp ->
  // netsnmp) exempts a same-token versioned subtree from the vendored drop.
  const auto built = buildGraphForPath(fixture("pass_self_project") / "net-snmp");
  REQUIRE(graph_contains(built.graph, "snmpwalk"));
  archcheck::scan::setSelfProjectDir({}); // reset — the root name matched a curated token
}

TEST_CASE("graph excludes external_libraries container subtree (#127)", "[graph][vendor]")
{
  // supercollider: bcp-trimmed Boost + ICU under external_libraries/, a name the
  // bare `external` token misses. Own QtCollider code must stay in.
  const TempTree tree = make_tree();
  write(tree.root / "QtCollider" / "QcMenu.h", "// author code\n");
  write(tree.root / "external_libraries" / "boost" / "asio.hpp", "// vendored boost\n");
  write(tree.root / "external_libraries" / "icu" / "utf.h", "// vendored icu\n");

  const auto built = buildGraphForPath(tree.root);

  REQUIRE(graph_contains(built.graph, "QtCollider/QcMenu.h"));
  REQUIRE_FALSE(graph_contains(built.graph, "external_libraries"));
  REQUIRE(built.graph.nodeCount() == 1);
}

TEST_CASE("graph excludes generated/copied files by header banner (#127)", "[graph][vendor]")
{
  // Coco/R banner + kernel UAPI: name-independent markers, no vendor dir, no path
  // marker. Own code mentioning generation/copying in prose must stay in (#131:
  // "copied from" is prose and was dropped — its own-FP class is asserted here).
  const TempTree tree = make_tree();
  write(tree.root / "Parser.h", "/* Compiler Generator Coco/R */\n// vendored\n");
  write(tree.root / "posix_types.h", "/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */\n");
  write(tree.root / "config_parser.h", "// hand-written parser; regenerate by hand if the DSL changes\n");
  write(tree.root / "util.h", "// helpers copied from the old impl, then rewritten by hand\n");

  const auto built = buildGraphForPath(tree.root);

  REQUIRE(graph_contains(built.graph, "config_parser.h")); // own code, prose only
  REQUIRE(graph_contains(built.graph, "util.h"));          // "copied from" prose is NOT a marker
  REQUIRE_FALSE(graph_contains(built.graph, "Parser.h"));
  REQUIRE_FALSE(graph_contains(built.graph, "posix_types"));
  REQUIRE(built.graph.nodeCount() == 2);
}
