# Clone detectors vs archcheck — landscape and demarcation

_Compiled 2026-06-06. Answers the recurring question "isn't this the same thing
as PMD CPD / SonarQube / jscpd?". The design source for our subsystem is
[../duplication_architecture.md](../duplication_architecture.md); the clone-type taxonomy
and formal methods are in [code_clones.md](code_clones.md); the TP empirics are in
[dedup_techniques_corpus.md](dedup_techniques_corpus.md)._

Why this doc exists: so we don't rebuild the reasoning every time. Short answer —
**comparing them to our detector is pointless: everyone's detector is the same
(token-based Rabin-Karp class). What you should compare is products.** They are "find clones".
archcheck is "keep architectural invariants in CI", where copy-paste is just **one of the
sensors** alongside cycles / SF.* / god-headers / levelization.

---

## 1. Summary table

| Tool | Technique | Clone types | Output | Runtime / packaging | How it gates CI | What it actually is |
|---|---|---|---|---|---|---|
| **PMD CPD** | tokens, Karp-Rabin¹ | 1, 2, partially 3 | list of blocks | JVM | exit≠0 by `--minimum-tokens` | standalone clone detector |
| **jscpd** | tokens, Rabin-Karp | 1, 2, partially 3 | list + % | Node | exit≠0 by `--threshold %` | standalone clone detector (now "AI-ready": MCP/skill) |
| **SonarQube** | own `sonar-cpd`² | 1, 2, partially 3 | **density %**, dashboard | server/platform | quality gate by **% density** of new-code (ignores <20 new lines) | quality platform (many metrics) |
| **Simian** | lines/text, dynamic pattern match | 1, 2 | list of blocks | JVM | line threshold | standalone clone detector (proprietary) |
| **Duplo** | lines (line-based) | 1 | list of blocks | standalone C++ (**abandoned**) | min line length | standalone clone detector |
| **CCFinderX** | suffix-tree, tokens | 2 | clones + genealogies | research (100 MLOC) | — | research tool |
| **NiCad** | text + norm + LCS | 3 (**precision reference 89–96%**) | clone classes | research (10 MLOC) | — | research tool |
| **Deckard** | AST subtrees | 3 | clones | research (1 MLOC) | — | research tool |
| **archcheck** | tokens, rare-index + token-LCS³ | 1, 2, 3 (Type-4 — no, deliberately) | **pairs `file:line↔file:line` + clone type + "extract to a shared spot"** | **single static binary** (libclang only) | advisory on a **new confirmed pair** (exit 0; hard gate is a later opt-in) + baseline | **architecture-invariant check**: a duplicate is 1 signal alongside cycles/SF.*/god-headers/levelization |

¹ PMD CPD went through three incarnations: Greedy String Tiling → Burrows-Wheeler → Karp-Rabin.
It can do `--ignore-identifiers` / `--ignore-literals` (Type-2), but normalization is **global**.
² SonarQube moved off PMD-CPD to its own `sonar-cpd`, works over "statements",
mutes indentation and string literals.
³ The SourcererCC line: an inverted index over rare tokens + token-LCS confirm.
See [../duplication_architecture.md §3](../duplication_architecture.md).

## 2. How archcheck actually differs (in decreasing importance)

1. **It's not a dup tool, it's an arch gate.** Duplication is one signal in one binary
   **alongside cycles, levelization, SF.7/8/9/21, god-headers**. Nobody in the
   list has this — they only know clones (or, like Sonar, all-metrics-everything
   on a server). This is the main difference; everything else is a consequence.
2. **A single static binary, no JVM/Node/server.** For CI it's "download and
   run". Competitors drag along a foreign runtime; the only standalone-C++ one (Duplo) is
   line-based and dead. → this is exactly the reason to write **our own** detector
   (there's nothing to wrap, see §4).
3. **Output is concrete pairs + what to fix**, not a percentage. Sonar fails by
   "density %" (an aggregate), we — by a concrete new pair with a note "extract to a
   shared header", in the Lakos *missing reuse edge* frame.
4. **Selective normalization for C++ idioms** (we keep call names and case labels).
   PMD/Sonar use global "drop all identifiers" normalization — on C++ it makes
   idiom-FP *worse* (every `switch` skeleton and `var=call(args)` collapses
   into one skeleton). This is our only real technical delta in the detector, and
   it makes sense **only because we gate** — precision for the gate, not for the dashboard.
5. **baseline from day one** — legacy is frozen, the gate catches only new things.
   (Sonar apparently can do this via new-code, the rest — no.)

## 3. Where we do NOT differ (honestly)

- **The detector itself** — an ordinary token-based Rabin-Karp class, like PMD/jscpd. No moat.
- **Type-2 normalization** — `--ignore-identifiers`/`--ignore-literals` in PMD,
  literal ignore in Sonar, per-language in Simian. "Ignore names/literals" is not ours.
- **Type-3 precision** — the reference is NiCad (89–96%), we don't beat it and don't try
  (Type-4 and AST cross-TU #052 are removed deliberately — we are intentionally *short of* the research tools).

## 4. The "own detector vs wrapper" fork — keep it honest

The archcheck differential stands on three legs: **selective-norm precision for the gate +
arch frame (pairs, not %) + a single binary.** If even one leg fails to fire —
for example, a measurement shows our precision on C++ is no better than PMD with `--ignore-literal` —
then the YAGNI-correct move is: **wrap jscpd/CPD instead of building our own**. Our own detector
is justified exactly by the distribution argument: under the contract "single static binary,
runtime only libclang" there's nothing to wrap (PMD/Simian = JVM, jscpd = Node, Sonar =
server, Duplo = line-based and abandoned). This is not "we have a better algorithm" — it's "there's nothing
to embed".

**In one sentence:** if you strip cycles/SF.*/levelization out of archcheck and
leave only dups — we're just another CPD, and it would be more correct to wrap jscpd.
The difference lives in the fact that duplicates are **not the main product, but one sensor on the
dashboard** of architectural invariants.

## Sources

- **PMD CPD** — <https://pmd.github.io/pmd/pmd_userdocs_cpd.html> ·
  algorithms/history: <https://github.com/pmd/pmd/blob/main/docs/pages/pmd/userdocs/cpd/cpd.md>
- **jscpd** — <https://github.com/kucherenko/jscpd> · <https://jscpd.dev/>
- **SonarQube** — quality gates: <https://docs.sonarsource.com/sonarqube-server/quality-standards-administration/managing-quality-gates/introduction-to-quality-gates>
  · "Manage Duplicated Code with Sonar": <https://www.sonarsource.com/blog/manage-duplicated-code-with-sonar>
- **Simian** — <https://simian.quandarypeak.com/>
- **Duplo** — <https://github.com/dlidstrom/Duplo>
- **NiCad / CCFinderX / Deckard** (BigCloneBench) —
  <https://clones.usask.ca/pubfiles/articles/SvajlenkoEvaluatingToolsICSME2015.pdf>
- **SourcererCC** (our index line) — <https://arxiv.org/pdf/1512.06448>
