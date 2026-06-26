# Kinds of copy-paste, their danger, and formal detection — a summary

_Compiled 2026-06-03. A literature summary + gap analysis: which of this the
archcheck code **already does**, and which it doesn't. The design source for the
subsystem — [../duplication_architecture.md](../duplication_architecture.md);
the TP/FP rules — task `backlog/new/071_fp_classification_rules.md` and
mem:`fp_classification_rules`; the TP empirics (real dedup refactorings in OSS) —
[dedup_techniques_corpus.md](dedup_techniques_corpus.md)._

Why this doc: the duplication subsystem grew out of practice (measurements on vmecpp /
LibreSprite / BambuStudio), not theory. Here is the external authoritative footing
(in the spirit of "authority over opinion"): how academia classifies copy-paste, how it
proves its harm, and by what formal methods it catches it. And a sober assessment of where
our code is already at the level of this frame, and where it isn't.

---

## 1. Taxonomy: 4 clone types

The canonical classification (Bellon, Koschke; cemented in the Roy & Cordy 2009 survey).
Everyone references it:

| Type | What it is | Divergence axis |
|-----|---------|-----------------|
| **Type-1** | identical code | only whitespace/comments |
| **Type-2** | structurally identical | identifiers, literals, types renamed |
| **Type-3** | an edited copy ("near-miss") | on top of Type-2 — lines inserted/deleted/changed |
| **Type-4** | semantic clone | same function, different text (different algorithm, `for`↔`while`) |

A subtlety from BigCloneBench: the zone between Type-3 and Type-4 is not binary — it's
broken into gradations (Very Strongly / Strongly / Moderately / Weakly Type-3). Useful if
we want a confidence scale instead of "clone / not a clone".

## 2. Why copy-paste is dangerous — empirics, not opinion

The main mechanism of harm is **not duplication itself, but desynchronization on
editing**. The supporting results:

- **Inconsistent change → defect.** Bettenburg et al. (WCRE'09): "almost every other
  *unintentional* inconsistent change of a clone leads to a bug". This is exactly the
  "clone consistency defect": you edit one copy and forget about its siblings. **The
  strongest signal of harm is the co-evolution of copies over history, not their textual
  similarity in a snapshot.**
- Cloned code is on average bug-prone; editing a method with clones is costlier — the cost
  grows with the share of the system touched by the clone group.
- **There is counter-evidence.** Krinke; Harder & Göde ("Cloned code: stable code") show:
  clones are often *more stable* than ordinary code and are maintained consistently. ⇒ the
  harm is **not universal**, it depends on the type and pattern of the clone.

## 3. When copy-paste is useful — the Kapser & Godfrey catalog

**"'Cloning considered harmful' considered harmful"** (Empirical Software Engineering,
2008, 13(6):645–692). They prove: copy-paste is often a **deliberate engineering
technique**. Their cloning patterns with the authors' assessment:

| Pattern | What it is | Assessment |
|---------|---------|--------|
| **Verbatim snippets** | a verbatim copy of a fragment | usually **harmful** |
| **Parameterized code** | a copy parameterizable into one function | usually **harmful** (this is exactly our TP) |
| **Boiler-plating** | a repeated structural skeleton (idioms) | usually **ok** |
| **Replicate & specialize** | copied and specialized for a context | usually **useful** |
| **Cross-cutting concerns** | duplication of system-wide functionality | usually **useful** |
| **Platform variations** | copies per OS/platform | acceptable |
| **Language idioms** | language idioms | acceptable |

Their thesis: deciding about a clone requires **contextual judgment**; the most
problematic patterns (verbatim, parameterized) should be eliminated, the rest are usually
beneficial.

This is **external support for our extractability test**: harm is determined not by textual
similarity, but by whether the copies evolve together and whether they can be meaningfully
extracted. Our "worst-kind TP" (copied a block, changed one token) = their *parameterized
code*. Our path guards (platform / perf / generated) = their *platform variations*,
*replicate & specialize*, plus generated code (not a clone at all — a compiler artifact).

## 4. Formal detection — by representation layers

The pipeline is the same for everyone: **preprocessing → intermediate representation →
comparison**. They differ by the representation (Roy & Cordy 2009 — a comparison framework):

| Approach | Representation | Catches | Boundary |
|--------|---------------|-------|---------|
| **Text / line** | raw lines | Type-1 | blind to renames |
| **Token** (SourcererCC, CCStokener) | a token stream | Type-1/2, fast | weak on near-miss without semantics |
| **AST** (Deckard) | a parse tree | Type-1/2/3, robust to ins/del | encodes syntax, not meaning |
| **PDG** | a data/control-flow graph | Type-4, non-contiguous | expensive |
| **Metric / hybrid / ML** | features / embeddings | Type-3/4 | recall ↑, cost ↑ |

Our choice — **token-based** (#056): cheap, no libclang, catches Type-1/2/3. Type-4 is
consciously out of scope for the fast backend (it needs PDG/AST). This is exactly the
"cheap-and-broad → expensive-and-precise" trade-off from the literature.

---

## 5. Gap analysis: which of this our code does

The state of the code as of 2026-06-03 (`src/scan/duplication/`).

### 5.1. Determining the **kind** of copy-paste — the capability exists, but it's dormant

`clone_classifier.{h,cpp}` → the function **`cloneType(a, b)`** already computes:

| `cloneType()` | → canonical type |
|---|---|
| `EXACT` | **Type-1** ✅ |
| `RENAMED` / `LITERAL` / `MIXED` | **Type-2** ✅ (even finer than the canon: what exactly was edited) |
| `STRUCTURAL` | **Type-3** ✅ |
| — | **Type-4** ❌ — out of scope by design (needs AST/PDG) |

**The main gap: `cloneType()` is never called in `scanForDuplication`.** In
`duplication_scanner.cpp` it's only `#include`-d; the `Pair` has no type field, the report
carries no type. That is, the classifier is implemented and covered by tests
(`tests/duplication_clone_classifier_test.cpp`), but **isolated from the pipeline**. To
"determine the kinds" at the output — all that's needed is to add `type` to `Pair` and call
the classifier in `phase3ScoreCandidates`. **Cheap (~30 min), a clean win.**

A finer classifier of the **nature of the edit** within Type-3 is also partially missing
(EDITED-LOGIC = an operator flip vs EXPANDED/SHRUNK = `+`/`-` dominate), described in
[../duplication_architecture.md](../duplication_architecture.md) §3.6 (#070 P3b).
`cloneType()` distinguishes what was edited (id vs lit), but not how the structure changed.

### 5.2. Determining the **importance** of copy-paste — almost none

A grep for `priority|severity|importance|rank` in the duplication code — **zero matches**.
Importance as a computed output is **not implemented**. This is consistent with
mem:`fp_classification_rules`: "similarity-score ≠ refactor-priority, never conflate".

The raw material for assessing importance is already in the code, but isn't folded into a
severity:

| Importance signal (from §2–3) | In the code? | How |
|---|---|---|
| size of the shared skeleton | partially | `Fragment.tokenCount`, `lcs` length — not aggregated into a weight |
| logic vs boilerplate | partially | `Fragment.diversity` (trigram) — but used as an **FP down-weight** (P1.1/P1.2), not as severity |
| number of copies (3+) | **no** | only **pairs** are detected; no clustering into clone classes (`wholeFileClones` — about something else) |
| extractability | **no** | this is still human judgment from the rules, not code |
| benign vs harmful | binary | path guards P0.7-0.9 = suppress/keep, not a gradation of importance |
| **inconsistent co-change** (the strongest!) | **no** | requires history; #054 (diff-mode) walks git, but measures drift, not the co-evolution of a clone group |

**Conclusion on importance:** the code measures **structural similarity in a snapshot**,
while the literature says the main predictor of harm is **the consistency of copy edits
over time** (Bettenburg). That measurement doesn't exist. That said, the infrastructure is
close: diff-mode #054 already knows how to walk git history without a checkout — that's the
natural place where the co-change of a clone group could be computed. The most valuable, but
also the most expensive, step.

### 5.3. Bottom line in one line

- **Kinds** — the code can do it (Type-1/2/3 via `cloneType`), but the capability isn't
  exposed; Type-4 — out of scope deliberately. → a cheap wire-in.
- **Importance** — the code **can't**: the ingredients exist (`tokenCount`, `lcs`,
  `diversity`, the index), there's no severity model and no co-change signal. → it needs
  design; the strongest signal (inconsistent co-change) lives in the plane of #054.

---

## Literature

- **Roy, Cordy, Koschke (2009)** — *Comparison and evaluation of code clone
  detection techniques and tools: A qualitative approach*, Sci. Comput. Program.
  74(7):470–495. + tech report 2007 "A Survey on Software Clone Detection
  Research". The main survey framework. <https://clones.usask.ca/clones/surveys/>
- **Kapser & Godfrey (2008)** — *"Cloning considered harmful" considered harmful:
  patterns of cloning in software*, Empirical Software Engineering 13(6):645–692.
  The counter-argument about useful clones → support for FP classification.
  <https://link.springer.com/article/10.1007/s10664-008-9076-6>
- **Bettenburg et al. (WCRE'09)** — *An Empirical Study on Inconsistent Changes to
  Code Clones at Release Level*. The main argument for "why catch them".
  <https://users.encs.concordia.ca/~shang/pubs/bettenburg-wcre09.pdf>
- **Mondal et al. (SAC'12)** / **Harder & Göde (2013)** — clones can be more stable
  than ordinary code; the harm is not universal.
  <https://www.cs.usask.ca/~croy/papers/2012/Mondal_SAC2012_Stability.pdf>
- **CCStokener (2023)**, **SourcererCC**, **Oreo** (arXiv 1806.05837) — token-based
  and "twilight zone" Type-3/4 detection.
</content>
