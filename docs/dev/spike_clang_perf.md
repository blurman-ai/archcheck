# Spike #043 — libclang perf on spdlog: is a fast-backend needed in v0.1

**Date:** 2026-05-29
**Goal:** a single number that settles the question "do we need a two-backend scheme in v0.1, or will libclang-only suffice?"
**Code:** [experiments/clang_perf/](../../experiments/clang_perf/) (throw-away, deleted after the spike).

## Setup

- Host: Astra Linux 1.7, GCC 8.3, CMake 3.18.4.
- libclang: **19.1.7** (`libclang-19-dev` from the astra2 repo). Version 11 is also present in the system, but there's no point measuring the old branch — archcheck will require ≥18.
- Target: **spdlog** (master, `2e71fdf3`) with tests + examples enabled → **141 translation units**. fmt master didn't work out — it requires CMake ≥ 3.27.
- Measurement: 3 runs of the spike's Release build, hot cache, single-thread.
- Metric: `clang_parseTranslationUnit` (with `SkipFunctionBodies | DetailedPreprocessingRecord | KeepGoing`) + `clang_getInclusions` — exactly what archcheck needs for the include graph and AST facts, without the overhead of parsing function bodies.

## Numbers

| Backend                              | Total wall | Median TU | Peak RSS |
|--------------------------------------|-----------:|----------:|---------:|
| **libclang-19** (parseTU + inclusions) | **14.9 s** | **73 ms** | **136 MB** |
| **regex baseline** (`#include`-grep)   |   **11 ms** | **0.04 ms** | **3.5 MB** |

Ratio: **libclang is ~1350× slower and ~40× heavier on memory.**

The numbers are stable across runs: spread of ±1.3% on wall, ±2% on RSS.

> **Important:** the regex baseline parses only top-level `.cpp` (`#include` lines without path resolution and without recursive descent into headers). A "real" fast-backend in archcheck must additionally resolve paths through `-I` and traverse transitive headers — that's ~10× overhead versus naive regex. So a realistic estimate for the fast-backend in our production code is: **~100 ms on spdlog**, not 11 ms.

## Scaling (rough extrapolation)

| Project size | libclang single-thread | libclang ×8 thread | Realistic fast-backend |
|----------------|-----------------------:|-------------------:|--------------------------:|
| ~150 TU (spdlog)  |   15 s |   2 s | 0.1 s |
| ~500 TU (mid-size)|   50 s |   7 s | 0.4 s |
| ~2000 TU (Boost-like) | 3.5 min | 30 s | 1.5 s |
| ~5000 TU (LLVM, Chromium core) | 9 min | 1.1 min | 4 s |

Per-TU parallelism is realistic (libclang `CXIndex` is not thread-safe by itself, but per-TU `CXTranslationUnit`s are independent; the standard scheme is one `CXIndex` per thread).

## Answer to the task's question

**Is a fast-backend needed in v0.1? Yes.**

Not because libclang is "unusable" — it's tolerable for CI of a typical project (~50s on 500 TU without parallelism, ~7s with). But because:

1. **Zero-setup remit.** Running `archcheck` with no arguments and without an LLVM dependency is part of the product promise (CLAUDE.md, `docs/architecture-spec.md`). libclang-only breaks this use case fundamentally.
2. **CI bottleneck.** On a monorepo with 5000 TU, libclang single-thread = 9 minutes on parsing alone. That's too expensive for a guard job that must fire on every PR within seconds. The fast-backend keeps this scenario in the seconds.
3. **Memory ceiling.** 136 MB peak on 141 TU = ~1 MB/TU of retained memory. At 5000 TU — ~5 GB peak (without aggressive `dispose` after each TU). This exceeds the default limit of a GitHub Actions free-tier runner (7 GB).
4. **Semantics aren't needed by everyone.** v0.1 rules (SF.7/8/9, Lakos: cycles/chains/god-headers) all run on the include graph without AST. Forcing the user to install libclang just for include-cycle detection is a user-experience regression.

## Recommendation for #042

The two-backend scheme, as laid out in #006 and #042, is **confirmed**. Specifically:

- Keep `--with-clang` as an opt-in flag.
- Keep the `IRule` (fast-backend) / `ISemanticRule` (libclang-backend) split from the phase-1 open question.
- libclang in v0.2 — as a "semantic preview": SF.21, then SF.2/5/10/11.
- In v0.3 — consider `--with-clang` as default-ON for the published Docker build (where libclang is already inside), but in the standalone binary it stays opt-in.

`--with-clang` is NOT removed. The scope of #042 proceeds as written.

## Reproducibility sanity-check

The spike builds with a single command given `libclang-19-dev` + GNU `time`. See [experiments/clang_perf/README.md](../../experiments/clang_perf/README.md). Run artifacts (`spike_run*.csv`, `baseline_run*.csv`) sit next to the code — they can be re-run and compared.
