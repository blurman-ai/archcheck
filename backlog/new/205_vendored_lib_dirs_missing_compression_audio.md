# 205 — Vendored dependency trees are scanned as author code

**Type:** bug (false positives in every signal)
**Status:** new
**Found:** 2026-08-23, on the 50-repo adopter sample (#203 groundwork)

## What is wrong

The vendored-code exclusion (#068 containers, #069 single-file libs, #109 version tails, #127
`external_libraries`, #164 fmt, #179 net-snmp) misses whole third-party trees in ordinary
projects. Measured on the 50-repo adopter sample: **14,637 files are scanned as the author's own
code that are a vendored dependency release dropped in whole.** The current lib list catches 142
files in the same trees, so it is roughly 1% of what is there.

Reproduced on a synthetic tree, one `using namespace std;` header per directory:

```
$ archcheck --format json <tree>
violations: 2
    SF.7 -> general/libzstd/hdr.h     <- vendored, should be dropped
    SF.7 -> src/hdr.h                 <- author code, correct
```

`third_party/libzstd/hdr.h` and `general/zlib/hdr.h` are both dropped, so the machinery works —
these names are simply absent.

## Three distinct gaps, in falling order of confidence

**1. Container-name variants.** `bylins/mud` keeps its dependencies in `src/third_party_libs/`
(libfort, nlohmann, pugixml). `normalizeDirSegment` yields `thirdpartylibs`; the list has
`thirdparty`. A `_libs`/`_lib` suffix on an already-listed container name defeats it.

**2. `extra/` as a container.** `logicalclocks/rondb` — the MySQL-family convention, and the
directory is nothing but dependencies:

    abseil base64 boost curl doxygen-awesome googletest gperftools icu libbacktrace
    libcbor libcno libedit libfido2 lz4 protobuf rapidjson tirpc unordered_dense xxhash

12,199 scanned files in one repo. `andrewnakas/Boxedwine64` does the same under `lib/`
(asmjit directx glew imgui mesa pugixml rtmidi sdl1 sdl2 simde softfloat tiny-process zlib) —
1,640 files, of which only `glew` and `zlib` are recognised today. `lib/` is riskier than
`extra/`: plenty of projects put their own code there.

**3. Library names.** Confirmed vendored under a non-container parent in this sample:

    sdl2 sdl1 eigen imgui rapidjson googletest gtest pugixml tinyxml2 bullet
    zstd xxhash lz4 protobuf curl abseil boost icu asmjit mesa simde

Plus, from `alphaonex86/CatchChallenger` commit `f73da0ee34` ("vendor libogg/libopus/
libopusfile/libxxhash/libzstd as plain files", 571,951 lines added, 316,025 in files archcheck
counts as the author's):

    libzstd libogg libopus libopusfile libxxhash

Name matching is the least safe of the three: `platform/sdl/` in Boxedwine64, `router/src/
protobuf/` in rondb, `src/bindings/lua/` in flux-core and `Render/Imgui/` in MiniEngine are the
project's own integration code with a library's name on the directory. The `selfProjectDir`
guard does not help — the project is not the library. Any name added here needs the same
curation the existing 16 got, and the shape-blind ones (`sdl`, `lua`, `imgui`, `protobuf`,
`openssl`) probably do not belong at all.

## Why it matters

Vendored code is the FP class this project has already spent five tasks on. Its clones,
god-headers and complexity are real text and pure noise. Compression libraries are the worst
case for the clone rules specifically: zstd and xxhash are full of near-identical unrolled blocks
by design, and `extra/` in one corpus repo puts 12k such files into the author's column.

## Not in scope

Whether `lib/` should be a container name. The evidence here is one repo, and the false-positive
cost (a project's own `lib/`) is high. Needs its own corpus pass.

## Acceptance criteria

- [ ] `thirdpartylibs` and the `_lib`/`_libs` suffix variants of existing container names dropped
- [ ] `extra/` decided either way, with the corpus count behind the decision recorded
- [ ] Curated library names added; each one checked against the "author's own integration dir"
      failure above
- [ ] Fixture per gap: container variant, vendored lib under a non-container parent, and a
      project whose own directory shares a library's name (must still be scanned)
- [ ] Self-project guard fixture: the library's own repo still scans itself
- [ ] `rondb`, `Boxedwine64`, `CatchChallenger` re-counted after the fix

## Evidence

`experiments/rerun_2026-08/adopters_sample.tsv` (the 50 repos); per-path counts reproduced with
`git ls-tree -r --name-only HEAD` filtered by `kProjectExtensions`, cross-checked by hand against
each tree's top-level listing — half the name matches were author code and were removed from the
count before it was written down.

---

## Addendum (2026-08-23) — the same gap in the BUILD-directory list

Found while cross-validating the clone detector against Duplo: the sample's top disagreement in
`numaes/protoCore` was a clone between
`build-dbg/CMakeFiles/3.28.3/CompilerIdCXX/CMakeCXXCompilerId.cpp` and
`build_nosurv/CMakeFiles/3.28.3/CompilerIdC/CMakeCCompilerId.c` — a CMake compiler-probe file,
scanned as the author's own code by BOTH tools.

`isExcludedDirName` matches `kExcludedDirNames` (`.git build .cache .idea .vscode out`) as an
**exact, case-sensitive** name, plus the `cmake-build-` prefix. Every other spelling passes:

```
$ archcheck --format json <tree>
    SF.7 -> build-dbg/x/h.h        <- committed build dir, scanned
    SF.7 -> build_check/x/h.h      <- committed build dir, scanned
    SF.7 -> src/h.h                <- author code, correct
                                   (build/ and cmake-build-debug/ correctly dropped)
```

Corpus count over the 50-repo adopter sample — 895 files in 3 repos:

| directory | files |
|---|---:|
| `build-dbg` `build_asan` `build_check` `build_nosurv` `build_off` | 157 each (protoCore) |
| `Build` | 105 (Boxedwine64) |
| `build_assert` | 5 (flux-core) |

**A bare `build*` prefix rule would be wrong.** `build_assert` in flux-core is authored source —
build-time assertion helpers, not a build tree. The safe form is the existing exact set plus
`build-` / `build_` followed by a known configuration token (dbg, debug, release, asan, tsan,
msan, check, test, ninja, gcc, clang, ...), or a case-insensitive exact match for `Build`/`BUILD`.
Anything looser trades a real FP class for a silent FN class.

Same acceptance shape as the main task: fixture per spelling, plus a fixture asserting that an
authored directory whose name merely starts with `build_` is still scanned.
