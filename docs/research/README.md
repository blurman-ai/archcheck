# Research — external architecture rule sources

A survey of widely recognized sources of architectural and design rules for C++ beyond what is already captured in [../architecture-spec.md](../architecture-spec.md). Each candidate has its own file in [rules/](rules/).

This is a **research catalog, not a roadmap.** Each rule here is a candidate for an archcheck default, which later needs to be:
1. re-evaluated in the context of the current MVP,
2. on selection — moved to `backlog/` as a task with fixtures,
3. implemented as an `IRule` under the corresponding source in `src/rules/`.

## Formalizability categories (backend type)

> **Note on taxonomies.** The architecture spec uses a separate
> classification of rules by source of architectural truth:
> `intrinsic / repo-inferred / user-declared`. That is a **different** taxonomy.
> To avoid confusing the reader, this catalog uses mnemonic
> **letters by backend**: `P / S / G / R / D / X`.

| Tag | What it means | Backend |
|---|---|---|
| **P** | Preprocessor / include scan / filesystem | fast backend (no libclang) |
| **S** | Semantic / AST | libclang |
| **G** | Graph metrics | graph (after P/S parsing) |
| **R** | Regex / text pattern | fast backend |
| **D** | Design-only — not statically checkable | — (deliberately omitted) |
| **X** | Out of scope — bugs / style / safety-cert | — (other tools) |

## Sources

| Source | Authority | What's useful | Link |
|---|---|---|---|
| C++ Core Guidelines, section SF | high | already in spec | [#S-source](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-source) |
| C++ Core Guidelines, section C (Classes) | high | C.121, C.133, C.134 | [#S-class](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-class) |
| C++ Core Guidelines, section I (Interfaces) | high | I.2, I.3, I.22 | [#S-interfaces](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-interfaces) |
| C++ Core Guidelines, section NL | high | NL.27 (file suffix) | [#S-naming](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-naming) |
| Lakos / Bloomberg BDE wiki | high | component naming, no inter-component friendship, declared external linkage | [bloomberg/bde wiki](https://github.com/bloomberg/bde/wiki/physical-code-organization), [isocpp blog](https://isocpp.org/blog/2020/02/C-Packaging-and-Design-Rules) |
| Martin package principles (REP/CCP/CRP/ADP/SDP/SAP) | high | SDP, SAP, CRP beyond existing Ce/Ca/I/A/D | [ADP wiki](https://en.wikipedia.org/wiki/Acyclic_dependencies_principle), [principles-wiki](http://principles-wiki.net/collections:robert_c._martin_s_principle_collection) |
| Google C++ Style Guide | medium | foreign forward-decls, include order groups, `using namespace` policy | [styleguide/cppguide](https://google.github.io/styleguide/cppguide.html) |
| HIC++ 4.0 | medium | `using`-decl in header, one-extern-class-per-header | [HIC++ PDF](https://abougouffa.github.io/awesome-coding-standards/hi-cpp-4.0.pdf) |
| clang-tidy (cppcoreguidelines/misc/google/llvm) | medium (impl reference) | `misc-no-recursion`, ready equivalents for SF.7/8/9/21/2 | [clang-tidy checks](https://clang.llvm.org/extra/clang-tidy/checks/list.html) |
| ArchUnit / Deptrac / SonarQube | medium (taxonomy) | default-deny + allow-list, layer templates, slice cycles | [ArchUnit](https://www.archunit.org/userguide/html/000_Index.html), [Deptrac](https://deptrac.github.io/deptrac/concepts/) |
| JSF AV C++ (Lockheed) | medium | mostly F; locally — fan-in/fan-out cap | [JSF AV PDF](https://www.stroustrup.com/JSF-AV-rules.pdf) |
| MISRA C++ / CERT C++ / AUTOSAR C++14 | — | **F (out of scope)** — safety-certification | — |

## Rule candidates

The full list is in [rules/](rules/). The grouping below is for navigation only.

### From C++ Core Guidelines (high authority)
- [no-mutable-globals](rules/no-mutable-globals.md) — I.2
- [no-singletons](rules/no-singletons.md) — I.3
- [no-complex-global-init](rules/no-complex-global-init.md) — I.22
- [interface-pure-abstract](rules/interface-pure-abstract.md) — C.121
- [no-protected-data](rules/no-protected-data.md) — C.133
- [uniform-access-level](rules/uniform-access-level.md) — C.134
- [file-suffix](rules/file-suffix.md) — NL.27

### From Lakos / Bloomberg BDE (high authority)
- [no-inter-component-friendship](rules/no-inter-component-friendship.md)
- [component-naming-prefix](rules/component-naming-prefix.md)
- [external-linkage-declared-in-header](rules/external-linkage-declared-in-header.md)
- [transitive-fan-in](rules/transitive-fan-in.md) — Lakos CD threshold

### From Martin package principles (high authority)
- [depends-on-less-stable](rules/depends-on-less-stable.md) — SDP
- [stable-but-concrete](rules/stable-but-concrete.md) — SAP
- [unused-symbols-from-dep](rules/unused-symbols-from-dep.md) — CRP

### From Google C++ Style (medium authority)
- [no-foreign-forward-decls](rules/no-foreign-forward-decls.md)
- [no-using-namespace-in-cpp](rules/no-using-namespace-in-cpp.md) — stricter SF.7
- [include-order-groups](rules/include-order-groups.md)

### From HIC++ 4.0 (medium authority)
- [no-using-decl-in-header](rules/no-using-decl-in-header.md) — 7.3.6
- [one-extern-class-per-header](rules/one-extern-class-per-header.md) — 3.3.1

### Extensions of existing metrics
- [god-namespace](rules/god-namespace.md) — extension of god-headers
- [no-recursion-across-modules](rules/no-recursion-across-modules.md) — clang-tidy mapped
- [forward-decl-of-std](rules/forward-decl-of-std.md) — UB per `[namespace.std]`
- [header-cpp-pairing](rules/header-cpp-pairing.md) — BDE pairing
- [deep-nested-namespace](rules/deep-nested-namespace.md) — sprawl detector
- [macro-defined-in-header](rules/macro-defined-in-header.md) — leakage detector

## Excluded — design-only (category D)

Deliberately not attempting to formalize:
- High cohesion within a module — subjective, requires human judgment.
- Right level of abstraction — not formalizable.
- DDD domain model purity — requires knowledge of domain logic.
- ADP cycle resolution by DIP — we detect cycles (SF.9), but *prescribing* DIP is not our job.
- CCP (classes that change together) — needs change history (git log), that's a different tool.
- REP (granule of release = granule of reuse) — needs release metadata, not static.
- Bloomberg "narrow contracts" — design talk, not AST.

## Out of scope (category X)

MISRA C++, CERT C++, AUTOSAR C++14, most of JSF AV (cyclomatic complexity, function length, lines-per-class) — these are safety/style. The spec [../architecture-spec.md](../architecture-spec.md) explicitly excludes them: embedded teams under strict certification pay for a catalog of someone else's rules, that's a different niche (PVS-Studio, Coverity).

## Structural findings

- **clang-tidy has already formalized** SF.7 (`google-build-using-namespace`), SF.8 (`llvm-header-guard`), SF.9 (`misc-header-include-cycle`), SF.21 (`misc-anonymous-namespace-in-header`), SF.2 (`misc-definitions-in-headers`), I.2 (`cppcoreguidelines-avoid-non-const-global-variables`), C.121/C.133/C.134 (`cppcoreguidelines-non-private-member-variables-in-classes`). In the archcheck report we can reference these IDs as `equivalent: clang-tidy ...` — this lowers the distrust threshold.
- **Bloomberg BDE wiki** — the most practical source after CCG SF. All cited rules are statically checkable; it makes sense to single it out as "Level 1.5" between SF and Lakos metrics.
- **Martin package principles** give 3 new checkable rules beyond Ce/Ca/I/A/D: ADP (= SF.9, already exists), **SDP-violation** (depend → less stable), **SAP-violation** (stable + concrete), **CRP** (unused symbols from dep). Marketing headline: *"checks all 6 Martin package principles where statically checkable"*.
- **ArchUnit/Deptrac/SonarQube** — a taxonomy reference, not new rules. Matches the spec.
- **Empirics, not rule sources** (a separate genre in this catalog): [code_clones.md](code_clones.md) — clone taxonomy + gap analysis; [dedup_techniques_corpus.md](dedup_techniques_corpus.md) — real dedup refactorings in OSS as ground truth for TP; [ai_drift_cases.md](ai_drift_cases.md) — AI commits as test cases for DRIFT rules.
- [ai_smell_signals.md](ai_smell_signals.md) — taxonomy of observable AI-smell signals: what to count as AI-assisted accretion, which signs are strong, which are weak, and which of them are even worth turning into `archcheck` rules.
- [goodfirstissue_dogfood.md](goodfirstissue_dogfood.md) — dogfooding archcheck on 10 live C++ projects from goodfirstissue.dev: exposed tool defects (vendor #127, SF.9 impl-idiom #126, SF.8 guard #128, extensionless #125) and real TP classes. Mature repos as a correctness test, not a demo showcase.
