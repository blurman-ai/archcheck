# [RES] 192: spec archaeology — when did "detects duplicated code" narrow to "detects duplicated functions"

**Created:** 2026-07-19
**Start date:** —
**Status:** new
**Module:** SCAN/DUPLICATION (research, read-only)
**Priority:** major
**Difficulty:** medium
**Blocks:** honest history section of any #190 write-up / outward post
**Blocked by:** —
**Related:** #190 (the gap itself — closed in working tree), #053 (line layer), #056 (token pass), #091 (maxTokens), #059 (precision)

## Goal

Establish, with commit-level receipts, **when and how the duplication subsystem's promise
narrowed** from "detects duplicated/copied code" to "detects duplicated function bodies" —
and whether that narrowing was a recorded decision or silent drift.

#190 (2026-07-18) established the *fact* of the gap: the fragmenter emitted a function
body whole and never descended into it, so a block copied *into* a function was invisible
as a class. This task establishes the *history*: was sub-function granularity ever
promised, when did the implementation fix function granularity, when did the docs start
saying "function-scale", and did any outward text promise more than shipped.

This is a READ-ONLY archaeology task. No code changes. No doc changes outside this file.

## Resolved facts (verified 2026-07-19 — do not re-derive, verify around them)

- The working tree of `/home/localadm/projects/cpparch` contains the **uncommitted** #190
  fix. Therefore: **HEAD (`13c8dfa`) = pre-fix state; working tree = post-fix.** All
  archaeology must use `git show` / `git log` against history. **You must NOT run
  `git checkout` / `git stash` / `git reset` — uncommitted work would be destroyed.**
- Pre-fix behavior at HEAD, `src/scan/duplication/fragmenter.cpp` (~line 215): a `{`
  preceded by `)` whose body fits `[minTokens, maxTokens]` is emitted whole, then
  `i = j + 1` — no descent. The header `include/archcheck/scan/duplication/fragmenter.h`
  says "Extract function-scale fragments" at HEAD.
- `docs/duplication_architecture.md` §3.2 at HEAD: "Function-scale fragments by the `{…}`
  balance heuristic". §9 at HEAD had: "a class/namespace in `[min,max]` is emitted whole
  instead of its methods" (understating the limitation as container-only).
- README at HEAD captions the demo GIF: "*A PR introduces a copied block*" and describes
  demo PRs including "*partial/structural near-misses*"; the demo repo PR names include
  `P3_large_block_copy`, `S1_partial_insert_midbody`, `S2_partial_inserted_block`. These
  are outward texts that read as block-level (sub-function) capability.
- Layer map (SSOT `docs/duplication_architecture.md` §2): #053 line layer / #056 token
  pass / #052 AST (not built) / #059 precision / #054 usage. Task write-ups live in
  `backlog/completed/`.
- The private companion journal `/home/localadm/projects/archcheck-journal/JOURNEY.md` is
  readable and contains the #190 discovery entries (2026-07-18). Read-only.

## Questions (each needs receipts: commit hash + file:line + verbatim quote)

- **Q1. What was promised at birth?** What wording did the original duplication tasks and
  specs use when the subsystem was introduced — "duplicate code", "copy-paste", "clones",
  any granularity qualifier? Sources: `backlog/completed/053*`, `backlog/completed/056*`
  (and any earlier duplication task they reference), `docs/architecture-spec.md`,
  `docs/MVP.md`, CHANGELOG entries that first mention duplication.
- **Q2. When did the implementation fix function granularity?** Find the commit that
  introduced `fragmenter.cpp` (`git log --oneline --follow --
  src/scan/duplication/fragmenter.cpp`, oldest entry) and quote its fragment-emission
  logic: did a descend-after-emit ever exist and get removed, or was emit-without-descend
  there from the first commit? If any later commit (e.g. #091 maxTokens 400→600) changed
  the granularity story, quote before/after.
- **Q3. When did the docs adopt "function-scale"?** First appearance of "function-scale"
  (and retirement of broader wording) in `docs/duplication_architecture.md`, the fragmenter
  header, `--help` text: `git log -S 'function-scale'`, `git log -S 'copied block'`,
  `git log -p -- docs/duplication_architecture.md` as needed.
- **Q4. Was the narrowing ever a recorded decision?** Search `docs/decisions/`,
  "Key decisions" sections of `backlog/completed/05*` and `backlog/completed/09*`,
  JOURNEY entries before 2026-07-18. Either quote the decision, or state plainly that no
  record exists (that IS a finding — silent drift).
- **Q5. Outward claims vs capability, dated.** When did the README GIF caption
  ("a copied block") and the demo-PR descriptions land (`git log -S 'copied block' --
  README.md`, `git log -S 'partial' -- README.md`)? For each outward block-level claim:
  date it, and state whether shipped behavior at that date could detect a sub-function
  block (per Q2 the answer is presumably "no" throughout — verify, don't assume).

## Method constraints

- History access via `git show <rev>:<path>` and `git log`/`git log -S`/`git log -p`
  ONLY. Absolute paths in all commands. Repo: `/home/localadm/projects/cpparch`.
- `git -c gc.auto=0` on every git invocation (background auto-gc corrupted batch reads in
  this environment before — known trap).
- Quote verbatim, short excerpts (≤10 lines per quote). Every quote must carry
  `commit-hash:path:line` or `commit-hash:path` provenance.
- If a question cannot be answered from the repo + companion journal, mark it
  `INSUFFICIENT_EVIDENCE` (per haiku_task_guide §2.5) — do not fill gaps with plausible
  narrative. Plausible-but-unreceipted is the failure mode this task exists to avoid.

## Do NOT

- Do NOT modify any file except this one (append `## Findings`).
- Do NOT commit, checkout, stash, reset, or touch the working tree state.
- Do NOT re-verify #190's technical claims (the gap, the fix) — that is done; this task
  is about the history of the promise, not the defect.
- Do NOT read `backlog/pending/`.
- Do NOT run archcheck, builds, tests, or corpus scans — pure archaeology.

## Deliverable (append to this file as `## Findings`)

1. **Timeline table**: date | commit | artifact (task/doc/code/README) | verbatim quote
   (short) | what it promised or implemented (code-clone vs function-clone).
2. **Per-question answers Q1–Q5**, each with receipts or `INSUFFICIENT_EVIDENCE`.
3. **Verdict** (one of): (a) deliberate narrowing — decision recorded at <commit/task>,
   (b) silent drift — first divergence between promise and implementation at
   <commit/date>, no decision record, (c) never promised — sub-function granularity was
   never claimed internally and the gap is outward-text-only, (d) INSUFFICIENT_EVIDENCE.
   State which artifacts the verdict rests on.
4. **Closure verdict** per haiku_task_guide §2.5 (ACCEPT / INSUFFICIENT_EVIDENCE / ...)
   with cited evidence lines.

## Definition of done (checkable without judgment)

- [x] Q1–Q5 each answered with ≥1 commit-hash receipt, or marked INSUFFICIENT_EVIDENCE.
- [x] The oldest fragmenter commit is identified by hash and its emission logic quoted.
- [x] The first outward "block"-level claim (README/demo) is identified by hash and date.
- [x] The verdict names its supporting artifacts explicitly.
- [x] No file modified except this one; no commits made (`git status` unchanged except
      this file).

## Findings

**Executed 2026-07-19. Read-only archaeology. No code/doc changes outside this file, no
commits. All history via `git -c gc.auto=0 show/log` on absolute paths.**

Headline: **the implementation was function-scale from its first commit, and the internal
engineering scope was explicitly "function-scale segments, not whole files" from the task
that specified it (2026-05-30).** Sub-function block granularity was never promised
internally; there was no broader implementation that got narrowed. The divergence lives in
(1) outward/README text that uses block-level language, and (2) an internal limitation note
in the arch doc that understated the boundary as container-only. Verdict = **(c)**, with a
documented outward-text-overstatement nuance.

### 1. Timeline table

| Date | Commit | Artifact | Verbatim quote (short) | Promised / implemented |
|------|--------|----------|------------------------|------------------------|
| 2026-05-30 | `0256032` | #056 task `backlog/wip/056_maj_fast_backend_partial_duplication_pass.md:61` | "**Гранулярность:** сегменты функционального масштаба, не целые файлы." (also l.92 "Разбить файлы на фрагменты функционального масштаба без парсера") | **Promise = function-scale**, explicitly contrasted with whole-file, not sub-function |
| 2026-05-31 | `3863364` | `docs/duplication_architecture.md` §3.2 | "Блок размера `[min_tokens, max_tokens]` эмитится как фрагмент; больше — спускаемся внутрь; меньше — отбрасываем." | **Design = emit in-range whole; descend ONLY if larger than max** (no descend-into for in-range bodies) |
| 2026-05-31 | `3863364` | `docs/duplication_architecture.md` §9 (l.199-200) | "**Сегментация** — `max_tokens` дробит блоки >~100 строк; класс/namespace в `[min,max]` эмитится целиком вместо методов." | Limitation stated as **container-only** — understated; the general sub-function-block case is absent |
| 2026-06-02 | `1df44f8` | `src/scan/duplication/fragmenter.cpp` (`collect`) + `fragmenter.h` | code: `if (fnBody && body>=min && body<=max) out.push_back(...) else collect(...descend...); i = j + 1;`; header: "// Extract function-scale fragments from tokenized source." | **Impl = emit-without-descend from the FIRST commit**; header already says "function-scale" |
| 2026-06-07 | `fd98704` | `#091`, `fragmenter.h` maxTokens 400→600 | task l.46 "600 keeps functions up to ~120 lines whole ... both twins stay whole and match" | Impl **doubled down on whole-function** granularity (opposite of sub-function descent) |
| 2026-06-26 | `823fe85` | `docs/duplication_architecture.md` (i18n) | English: "a class/namespace in `[min,max]` is emitted whole instead of its methods" | English translation of the **understated container-only** limitation |
| 2026-07-02 | `05a25b0` | `README.md:11,62-63` (#163 demo GIF) | "*A PR introduces a copied block*"; "*partial/structural* near-misses" | **Outward text = block-level language** (reads as sub-function capability) |
| 2026-07-18 | uncommitted (#190) | `archcheck-journal/JOURNEY.md:3229` | "sub-function копипаст не детектится вообще: ядро `--duplication` уже полтора месяца обещает больше, чем делает" | Discovery of the gap; author's own retrospective |

### Q1 — What was promised at birth?

Two registers, both receipted:

- **Engineering scope (narrow, explicit):** #056 task, first commit `0256032`
  (2026-05-30), `backlog/wip/056_maj_fast_backend_partial_duplication_pass.md:61`:
  "**Гранулярность:** сегменты функционального масштаба, не целые файлы." (l.92: "Разбить
  файлы на фрагменты функционального масштаба без парсера"). The scope was pinned to
  *function-scale*, and the only contrast drawn was **against whole files** — going finer
  than files. Sub-function blocks were never named as an in-scope granularity.
- **Positioning / conceptual register (loose "block"/"copy-paste"):** #053 (dropped,
  line layer) `backlog/dropped/053_maj_fast_backend_line_duplication_pass.md:16-18`:
  "duplicated blocks as a **missing reuse edge** ... the headline signal is concrete
  findings (long **blocks**)"; #056 l.22-23 "the main painful class of **copy-paste** is
  different"; `docs/architecture-spec.md` frames duplication as GitClear-style "copy-paste"
  (l.68) and ships `--duplication` as a "token clone detector" (l.510, 595); `docs/MVP.md:38`
  "**Copy-paste** surfaced in PR diff". These use "block"/"copy-paste"/"clone" generically,
  never with a sub-function qualifier and never with a claim to detect a fragment *inside* a
  function.

Verdict on Q1: birth promise = **function-scale, explicitly not sub-file**; "block" and
"copy-paste" appear only as generic positioning, never as a sub-function capability claim.
Receipts: `0256032:...056...:61`, `3863364:docs/duplication_architecture.md` §3.2.

### Q2 — When did the implementation fix function granularity?

**From the very first commit — descend-after-emit never existed.** Oldest fragmenter
commit: `1df44f8` (2026-06-02, "port core modules from #056 spike"). Its `collect()` in
`1df44f8:src/scan/duplication/fragmenter.cpp`:

```
const bool fnBody = (i > 0 && t[i - 1].sym == ")");
if (fnBody && body >= opt.minTokens && body <= opt.maxTokens)
  out.push_back(makeFragment(t, i + 1, j, file, lines));
else
  collect(t, match, i + 1, j, opt, file, lines, out);
i = j + 1;
```

An in-range function body is emitted whole, then `i = j + 1` skips past its closing brace —
**no descent into it.** Descent (`else` branch) fires only when the body is NOT a function
body or is OUT of `[min,max]`. So the #190 behavior is present from commit #1; there was no
earlier descend-after-emit that was removed. The header at `1df44f8` already reads
"// Extract function-scale fragments from tokenized source. A `{` preceded by `)` ... we
emit such blocks if they fall within `[minTokens, maxTokens]`. Otherwise we descend..." —
i.e. the emit-in-range-without-descend contract was documented in the code from day one.

Later commit `fd98704` (#091, 2026-06-07) changed **only** `FragmentOptions::maxTokens`
400→600 and did **not** change the granularity story — it raised the cap so larger *whole*
functions stay whole (`backlog/completed/091_maj_dup_large_function_recall.md:46` "600 keeps
functions up to ~120 lines whole"). This moved the design *further* toward whole-function,
not toward sub-function descent.

Receipts: `1df44f8:src/scan/duplication/fragmenter.cpp` (collect), `1df44f8:include/archcheck/scan/duplication/fragmenter.h`, `fd98704` + `backlog/completed/091...:46`.

### Q3 — When did the docs adopt "function-scale"?

- **Concept present from the arch-doc's birth**, in Russian: `3863364`
  (2026-05-31) `docs/duplication_architecture.md` §3.2 "Фрагменты функционального масштаба
  по эвристике баланса `{…}`". The design was function-scale before the code existed.
- **The English string "function-scale"** entered the codebase in the **code header** at
  `1df44f8` (2026-06-02, "// Extract function-scale fragments"), and entered the **docs**
  via the i18n translation of the arch doc (`823fe85`, 2026-06-26, #150 P0+P1) and backlog
  (`9ba1bef`, #150 P2+P3) — `git -c gc.auto=0 log -S 'function-scale'` returns exactly these
  code/i18n commits and no earlier English introduction.
- No commit ever "retired broader wording" in favor of "function-scale" — there was no
  broader wording to retire; the doc was function-scale from `3863364`.

Receipts: `3863364:docs/duplication_architecture.md` §3.2, `1df44f8:...fragmenter.h`, `823fe85`, `9ba1bef` (via `log -S 'function-scale'`).

### Q4 — Was the narrowing ever a recorded decision?

**No decision record exists — because there was no narrowing to decide.** Specifically:

- `docs/decisions/` contains only `001-config-rules-deferred-to-v0.2.md`,
  `002-sf21-deferred-to-v0.2.md`, `003-fast-backend-default.md`. None concerns duplication
  granularity (`grep -rn -i 'function-scale|granular|duplicat' docs/decisions/` → no
  granularity hit). So there is no ADR narrowing "code" to "functions".
- However, function-scale **was** a recorded engineering *scope* (not a *narrowing
  decision*): #056 task l.61 (`0256032`) and arch doc §3.2 (`3863364`). The scope was set
  at birth, never changed, so "narrowing" is the wrong frame.
- The one internal artifact that touches the sub-function gap is the arch-doc §9
  limitation, and it **understated** it: `3863364` (2026-05-31) l.199-200 "класс/namespace в
  `[min,max]` эмитится целиком вместо методов" — framed as a container-only quirk, not the
  general sub-function-block case. This mis-scoped note is a documentation defect, not a
  decision record.
- **JOURNEY before 2026-07-18:** no entry records the sub-function gap as a known/accepted
  boundary. The relevant granularity entries are (a) `JOURNEY.md:879` — the iterative-DFS
  rewrite of `collect()` for deep-nesting stack overflow (commit `8133550`), which
  explicitly preserved fragment output "byte-for-byte identical" and thus did **not** touch
  the emit/descend contract; and (b) `JOURNEY.md:3229`, dated **2026-07-18**, the #190
  discovery itself: "ядро `--duplication` уже полтора месяца обещает больше, чем делает" and
  l.3300 confirming the §9 wording "занижала проблему до причуды контейнеров" (understated
  the problem to a quirk of containers). No pre-2026-07-18 record exists.

Verdict on Q4: **no decision record**; function-scale was a birth-time scope, and the
sub-function gap was surfaced only on 2026-07-18. Receipts: `docs/decisions/` listing,
`0256032:...056...:61`, `3863364:...§9`, `archcheck-journal/JOURNEY.md:3229,3300,879`.

### Q5 — Outward claims vs capability, dated.

- **README "copied block" caption + "partial/structural near-misses":** landed
  `05a25b0` (2026-07-02, #163), `README.md:11` "*A PR introduces a copied block*" and
  l.62-63 "partial/structural near-misses". Shipped fragmenter behavior on that date =
  emit-without-descend (unchanged since `1df44f8`; not fixed until #190, 2026-07-18).
  Therefore, for a block copied *inside* a function whose body fits `[min,max]`, shipped
  behavior on 2026-07-02 **could not** report it as a clone. The generic phrase "copied
  block" thus overstates for the sub-function case throughout 2026-07-02 → 2026-07-18.
- **Demo-PR names** (`P3_large_block_copy`, `S1_partial_insert_midbody`,
  `S2_partial_inserted_block`): whether each *specific* demo PR is a sub-function insert
  (and hence would/would not fire) is **not establishable from this repo** — the demo lives
  in an external repo and the fixtures are not in-tree. Marked **INSUFFICIENT_EVIDENCE** for
  the per-PR claim. What is established: the *naming* ("partial insert midbody", "inserted
  block") uses sub-function language, and the general capability it implies was not shipped
  before #190.
- Earlier outward duplication language ("block"/"copy-paste") in spec/MVP/#053/#056
  predates and matches Q1; none of it made a dated *sub-function* capability claim beyond
  the generic "block" register.

Receipts: `05a25b0:README.md:11,62-63`; capability per Q2 (`1df44f8` unchanged to HEAD).

### 3. Verdict

**(c) — never promised.** Sub-function block granularity was never claimed as an internal
capability: the #056 task fixed the scope at "function-scale segments, not whole files"
(`0256032:...056...:61`, 2026-05-30), the arch doc prescribed emit-in-range-without-descend
from birth (`3863364` §3.2, 2026-05-31), and the code implemented exactly that from its
first commit (`1df44f8`, 2026-06-02) with no earlier broader version to narrow from. #091
(`fd98704`) moved further toward whole-function, confirming direction of intent.

This is **not** (a): no ADR or decision narrowed the scope. It is **not** (b) in the
implementation sense: there was no divergence-over-time between promise and code — both were
function-scale throughout; nothing drifted. The gap that #190 found is real but lives in two
places outside the core promise:
1. **Outward text** — README "copied block" / "partial...near-misses" (`05a25b0`,
   2026-07-02) uses block-level language that a reader maps onto sub-function capability the
   tool did not have.
2. **An internal doc defect** — the arch-doc §9 limitation understated the boundary as
   container-only ("class/namespace ... emitted whole instead of its methods", `3863364` →
   `823fe85`), hiding the general sub-function-block case rather than recording it.

The verdict rests on: `0256032` (#056 scope), `3863364` (arch-doc design + §9), `1df44f8`
(first-commit code + header), `fd98704`/`#091` (direction), `05a25b0` (README), and
`JOURNEY.md:3229` (discovery date). The label "code-clone → function-clone narrowing" is
itself imprecise: internally it was function-scale from t0; only the *outward* register ever
read as broader.

### 4. Closure verdict (haiku_task_guide §2.5)

**ACCEPT** — for the archaeology deliverable (this is a read-only research task; "target
scenario" = answer Q1-Q5 with receipts and reach a receipted verdict).

- Q1-Q5 each answered with ≥1 commit-hash receipt; Q5's per-demo-PR sub-claim explicitly
  marked INSUFFICIENT_EVIDENCE (external repo, not in-tree).
- Oldest fragmenter commit identified (`1df44f8`) and its emission logic quoted verbatim.
- First outward block-level claim identified and dated (`05a25b0`, 2026-07-02).
- Verdict (c) names its supporting artifacts explicitly (§3).
- No file modified except this task file; no commits, no checkout/stash/reset; all history
  read via `git show`/`git log`.

**Unverified / doubts surfaced:** (1) whether specific demo PRs `P3/S1/S2` would fire under
shipped behavior on 2026-07-02 — external repo, not archaeologically decidable here
(INSUFFICIENT_EVIDENCE). (2) The verdict distinguishes "function-scale was the internal
promise" (well-receipted) from "the §9 note understated the limitation" (a doc defect, also
receipted) — both hold simultaneously; if the reviewer considers the understated §9 note to
constitute an *internal* mis-promise rather than a mere doc defect, the label shifts toward
"(c) plus an internal-doc-accuracy finding", not toward (a)/(b).

## Verification (2026-07-19, author-side)

Spot-checked 4/4 headline receipts against `git show` — all verbatim matches:

- `0256032` #056 task: "Гранулярность: сегменты функционального масштаба, не целые файлы." ✓
- `1df44f8` is the oldest `--follow` entry for `fragmenter.cpp`; its `collect()` emits an
  in-range `fnBody` without descending (else-branch descends only for non-fn/out-of-range) ✓
- `3863364` arch doc: l.70 "эмитится как фрагмент; больше — спускаемся внутрь" and l.200
  "эмитится целиком вместо методов" ✓
- `05a25b0` dated 2026-07-02, README caption "A PR introduces a copied block" ✓

Adversarial check found **one coverage gap** (Q3 listed `--help`, Findings skipped it):
`src/main.cpp:36` "report **duplicate code**; advisory" — landed `5b5fb27` (2026-06-05,
#082 "promote --duplication to shipped advisory capability"). Same generic outward
register as README/spec ("code", no granularity qualifier); it predates the README caption
as the earliest CLI self-description saying "code". Does not change verdict (c) — it
strengthens the "outward register said code, internal scope said functions" tension and
adds a third dated outward artifact (2026-06-05) beside README (2026-07-02) and the
understated §9 note (2026-05-31). CHANGELOG checked too: "block"/"copies"/"clone" in the
same generic register, no explicit sub-function claim.

Verdict (c) **confirmed by verification**.
