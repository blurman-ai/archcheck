#include <catch2/catch_test_macros.hpp>

#include "archcheck/rules/area_of.h"

using archcheck::rules::areaOf;

// Pins the CURRENT behavior of the module-attribution heuristic that feeds the
// lateral-drift (#115) and bidirectional-coupling rules. It had no direct unit
// test; this is the safety net for the #154 consolidation of its dir lists,
// which overlap (and drift from) scan/file_classification.h.

TEST_CASE("areaOf: first path component after stripping wrapper dirs", "[rules][area]")
{
  CHECK(areaOf("src/graph/builder.cpp") == "graph");
  CHECK(areaOf("include/archcheck/scan/x.h") == "archcheck");
  CHECK(areaOf("graph/builder.cpp") == "graph"); // no wrapper prefix
  CHECK(areaOf("source/Irrlicht/scene.cpp") == "Irrlicht");
}

TEST_CASE("areaOf: root-level files have no module", "[rules][area]")
{
  CHECK(areaOf("main.cpp").empty());
  CHECK(areaOf("src/main.cpp").empty());       // file directly under a wrapper
  CHECK(areaOf("include/lib/only.h").empty()); // every segment is a wrapper
}

TEST_CASE("areaOf: noise/scaffolding dirs yield no module", "[rules][area]")
{
  CHECK(areaOf("build/x.cpp").empty());
  CHECK(areaOf("third_party/fmt/format.h").empty());
  CHECK(areaOf("test/foo_test.cpp").empty());
  CHECK(areaOf("node_modules/x/y.js").empty());
  CHECK(areaOf("src/build_tools/gen.cpp").empty());  // build_ prefix
  CHECK(areaOf("mockserver/x.cpp").empty());         // mock prefix
  CHECK(areaOf("src/override_layer/x.cpp").empty()); // "override" substring
}

// #154 merge: area_of now delegates vendored/test/excluded dir classification to
// file_classification.h's canonical predicates (no more duplicated, drifting list),
// so these dirs are noise → no module. Verified no-op for the lateral-drift /
// coupling rules: the graph already drops vendored/test files via the same
// predicates (authored_scope), measured 0/100427 authored corpus files change areaOf.
TEST_CASE("areaOf: vendored/test dirs are not a module (shared with file_classification)", "[rules][area]")
{
  CHECK(areaOf("qhull/geom.cpp").empty());     // kVendoredLibDirs
  CHECK(areaOf("submodules/m/x.cpp").empty()); // kVendoredDirNames
  CHECK(areaOf("deps/foo/x.cpp").empty());     // kVendoredDirNames (old noiseDirs had _deps only)
  CHECK(areaOf("tst/x.cpp").empty());          // kTestDirNames
  CHECK(areaOf("third-party/k/x.h").empty());  // normalized -> thirdparty
  CHECK(areaOf("freetype/ft.c").empty());      // vendored lib name
}
