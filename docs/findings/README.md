# Findings on real code

Include cycles archcheck flagged on well-known C++ projects — the hard part of the gate, and the
only finding here that fails a build. Each is pinned to an exact commit, quoted in both directions
with `file:line`, and reproducible with the command shown. A cycle either exists in the include
graph or it doesn't, so these are facts rather than heuristics — but every one carries an honest
note on why a maintainer might have made the call on purpose. Below them sit the **advisories** —
copy-paste and complexity growth, caught in a single commit's diff, and never gating.

Reproduce any of them:

```bash
git clone <repo> && cd <repo> && git checkout <sha>
archcheck . | grep SF.9      # isolate the cycle line(s); a full run also prints advisories
```

(The full `archcheck .` output on a large project is long — chain-length / god-header / SF.7-8
advisories alongside the cycles. Piping through `grep SF.9` shows just the cycle lines, and the
count matches the "N SF.9 cycles" stated per finding below.)

---

## folly — `Future.h` ⇄ `Promise.h`

`facebook/folly`, commit
[`0985312`](https://github.com/facebook/folly/tree/0985312ed8e707a75185ad96aa626c714e4fb7cd).
Two of Meta's most-used C++ names form an include cycle.

```
folly/futures/Future.h: [SF.9] cycle: folly/futures/Future.h → folly/futures/Promise.h → folly/futures/Future.h
```

[`Future.h:39`](https://github.com/facebook/folly/blob/0985312ed8e707a75185ad96aa626c714e4fb7cd/folly/futures/Future.h#L39)
includes `Promise.h` in the normal dependency block. [`Promise.h:490`](https://github.com/facebook/folly/blob/0985312ed8e707a75185ad96aa626c714e4fb7cd/folly/futures/Promise.h#L490)
includes `Future.h` back, at the very bottom of the file, after `} // namespace folly`:

```cpp
// Promise.h, last lines
488:} // namespace folly
489:
490:#include <folly/futures/Future.h>
491:#include <folly/futures/Promise-inl.h>
```

Both headers carry `#pragma once`, so the preprocessor terminates. The cycle stands anyway:
neither header can be compiled, reasoned about, or swapped independently of the other.

**Counter-argument.** This uses folly's own trailing-include idiom (declare the class, close the
namespace, then include the file with the out-of-line bodies that need the other type). A folly
maintainer would fairly say Promise and Future are one coupled pair by design. That is the point
the finding makes concrete: the idiom, however deliberate, produces a Lakos cycle by definition,
and archcheck names it instead of leaving it implicit.

---

## RocksDB — `db.h` ⇄ `multi_scan.h`

`facebook/rocksdb`, commit
[`a214d3f`](https://github.com/facebook/rocksdb/tree/a214d3f1f8d46164f953498be82e4aa96c8779a0).
The primary public header cycles through an `EXPERIMENTAL` feature header.

```
include/rocksdb/db.h: [SF.9] cycle: include/rocksdb/db.h → include/rocksdb/multi_scan.h → include/rocksdb/db.h
include/rocksdb/db.h: [Lakos.GodHeader] fan-in 110 exceeds threshold 50
```

[`db.h:25`](https://github.com/facebook/rocksdb/blob/a214d3f1f8d46164f953498be82e4aa96c8779a0/include/rocksdb/db.h#L25)
includes `multi_scan.h`; [`multi_scan.h:8`](https://github.com/facebook/rocksdb/blob/a214d3f1f8d46164f953498be82e4aa96c8779a0/include/rocksdb/multi_scan.h#L8)
includes `db.h` back on its first non-comment line.

One direction is defensible: `db.h`'s inline `NewMultiScan()` default implementation calls
`std::make_unique<MultiScan>` and needs the complete type. The other is not. `multi_scan.h`
uses `DB` only as a pointer (`DB* db_`, `DB* db` parameters) and never calls a `DB` member
inline — the sole `db->` in the file sits in a comment. `db.h` already forward-declares
`class MultiScan;` at line 55, so the reverse include is avoidable: a `class DB;` forward
declaration in `multi_scan.h` would break the cycle with no call-site changes.

**Counter-argument.** A two-header cycle around a still-`EXPERIMENTAL` feature is a small, local
finding, not systemic rot. It is shown here for its public-API blast radius (the cycle drags the
experimental header into everything that depends on `db.h`, fan-in 110) and because the fix is
provably one forward declaration.

---

## Windows Terminal — `Utils.h` ⇄ `SettingContainer.h`

`microsoft/terminal`, commit
[`47e255f`](https://github.com/microsoft/terminal/tree/47e255f0579142a2bec6c308bf4382b3062407ff)
(re-verified against main @ [`922beefb`](https://github.com/microsoft/terminal/commit/922beefb83764646331662d6d15d70107d556402), 2026-07-14).
Both edges of the cycle are dead includes.

```
src/cascadia/TerminalSettingsEditor/SettingContainer.h: [SF.9] cycle: …/SettingContainer.h → …/Utils.h → …/SettingContainer.h
```

[`SettingContainer.h:21`](https://github.com/microsoft/terminal/blob/47e255f0579142a2bec6c308bf4382b3062407ff/src/cascadia/TerminalSettingsEditor/SettingContainer.h#L21)
includes `Utils.h`, but `SettingContainer.h`/`.cpp` use none of its macros or functions
(`GetSelectedItemTag`, `LocalizedNameForEnumName`, `DismissAllPopups`, `HasScrollViewer`, or any
`*_BINDABLE_ENUM_SETTING` macro). The macros it does use, `DEPENDENCY_PROPERTY` and
`BASIC_FACTORY`, live in `cppwinrt_utils.h` and arrive through `pch.h`.
[`Utils.h:6`](https://github.com/microsoft/terminal/blob/47e255f0579142a2bec6c308bf4382b3062407ff/src/cascadia/TerminalSettingsEditor/Utils.h#L6)
includes `SettingContainer.h` back, and that edge is dead too. In all 149 lines of `Utils.h`,
`SettingContainer` appears only on that `#include` line:

```
$ grep -in settingcontainer src/cascadia/TerminalSettingsEditor/Utils.h
6:#include "SettingContainer.h"
```

That line was added by PR [#19519 "Implement search in Settings UI"](https://github.com/microsoft/terminal/pull/19519),
commit [`f20c549d1`](https://github.com/microsoft/terminal/commit/f20c549d1) (2026-02-20, Carlos
Zamora, Microsoft). The same commit added `BringIntoViewWhenLoaded()` to the `HasScrollViewer<T>`
mixin, which locates elements through `FindName(elementName)` by string, not through
`SettingContainer`. The include line did not exist before that commit. The cycle traces to one
recent PR, and the include it added was never used from the day it landed. Removing either edge
breaks the cycle with no call-site impact.

**Counter-argument.** This is a WinRT/XAML settings module, one cycle in one pair, not a claim
about the whole ~500K-line codebase (archcheck reports 2 SF.9 cycles in that checkout). The value
here is precision: the finding points at one forgotten line from one identifiable PR, not at a
vague style complaint.

---

## Advisories — copy-paste and complexity growth

Everything past the cycles is advisory: archcheck reports it and never fails the build. These are
judgment calls rather than graph facts, and a maintainer can wave either one through. They earn a
place here because they are the ordinary way a codebase drifts — one merged commit at a time — and
archcheck catches them in the diff of the commit that introduces them, where a file-by-file review
tends not to look. Reproduce either with `--diff`:

```bash
git clone <repo> && cd <repo>
archcheck --diff <sha>^..<sha> . | grep NEW_CLONE        # copy-paste added in that commit
archcheck --diff <sha>^..<sha> . | grep LOCAL_COMPLEXITY # complexity grown in that commit
```

### Copy-paste — a DuckDB extension names it "unified"

`teaguesterling/duckdb_webbed`, a DuckDB community extension for reading XML and HTML, commit
[`10bf964`](https://github.com/teaguesterling/duckdb_webbed/commit/10bf96497425c413a129f89a08b558b428b2c97a),
titled *"Phase 2: Implement internal unified functions for XML/HTML parsing"* — a single commit of
+521 lines into one file. The message says the XML and HTML paths were unified; archcheck reports
the new "unified" functions as copies of the existing XML ones with cosmetic edits — fourteen clones
in the one commit.

```
src/xml_reader_functions.cpp:187: DRIFT.NEW_CLONE — copy-paste introduced (EXACT): lines 187-216 — clone of src/xml_reader_functions.cpp:756-785
src/xml_reader_functions.cpp:441: DRIFT.NEW_CLONE — copy-paste introduced (STRUCTURAL): lines 441-530 — clone of src/xml_reader_functions.cpp:920-1002
```

The EXACT pair (an option parser for `force_list`) is identical token for token, comments and all.
The STRUCTURAL pair is a ~90-line scan-function body whose only inserted difference is one line at
[`:451`](https://github.com/teaguesterling/duckdb_webbed/blob/10bf96497425c413a129f89a08b558b428b2c97a/src/xml_reader_functions.cpp#L451):

```cpp
const bool is_html = (bind_data.parse_mode == ParseMode::HTML);
```

That single flag stands in for what should have been one function taking a mode argument.

**Counter-argument.** XML and HTML parsing do share structure, and "copy it working, unify later"
is a normal first pass. Fair — except the commit message already claims the unification is done, so
the two bodies will drift under a name that says they cannot. The clone is trivially extractable
(the bodies differ by that one `is_html`), which is what separates a real copy-paste from
coincidental boilerplate: a fix made in the XML copy will not reach the HTML one.

### Complexity growth — RocksDB keeps feeding a 648-line function

`facebook/rocksdb`, commit
[`62f0562`](https://github.com/facebook/rocksdb/commit/62f05627befadf88d47aa62a6c4fc10375660581),
*"Reduce manifest rotation for foreground metadata ops (#14797)"*.

```
db/version_set.cc:5897: DRIFT.LOCAL_COMPLEXITY — function 'ROCKSDB_NAMESPACE::VersionSet::ProcessManifestWrites' grew local complexity from 314 to 318 (+4, already above 25)
```

[`ProcessManifestWrites`](https://github.com/facebook/rocksdb/blob/62f05627befadf88d47aa62a6c4fc10375660581/db/version_set.cc#L5897)
is already 648 lines and six control-flow levels deep, at a local complexity of 314 — thirteen times
the threshold. This commit adds a reasonable feature: relax the MANIFEST size limit by 25% when a
write batch carries a foreground operation, so a user action is less likely to block on rotation. It
lands as a new scan loop and branch, and the addition even books its own deferred work:

```cpp
bool has_foreground_operation = false;
for (const VersionEdit* e : batch_edits) {
  if (e->IsForegroundOperation()) { has_foreground_operation = true; break; }
}
// TODO/future: ... trigger background manifest rotation if we are beyond the soft limit ...
```

The reviewer sees a self-contained, well-commented block. archcheck sees a function long past
holding in one head grow again — with a `TODO` filed against the code that most needs splitting
instead of more accretion.

**Counter-argument.** +4 on 314 is a rounding error, and the feature is correct and local. True, and
archcheck is not arguing with the change. `LOCAL_COMPLEXITY` speaks only when an
already-over-threshold function grows, so it reads as a nudge — extract here before adding the next
branch — never a demand to rewrite. Being advisory, it never blocks the PR.
