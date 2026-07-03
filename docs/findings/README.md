# Findings on real code

Include cycles archcheck flagged on well-known C++ projects. Each is pinned to an exact commit,
quoted in both directions with `file:line`, and reproducible with the command shown. A cycle
either exists in the include graph or it doesn't, so these are facts rather than heuristics —
but every one carries an honest note on why a maintainer might have made the call on purpose.

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
[`47e255f`](https://github.com/microsoft/terminal/tree/47e255f0579142a2bec6c308bf4382b3062407ff).
A generic utility header depends on a specific UI control it never names.

```
src/cascadia/TerminalSettingsEditor/SettingContainer.h: [SF.9] cycle: …/SettingContainer.h → …/Utils.h → …/SettingContainer.h
```

[`SettingContainer.h:21`](https://github.com/microsoft/terminal/blob/47e255f0579142a2bec6c308bf4382b3062407ff/src/cascadia/TerminalSettingsEditor/SettingContainer.h#L21)
includes `Utils.h` for its enum-binding macros — a real dependency.
[`Utils.h:6`](https://github.com/microsoft/terminal/blob/47e255f0579142a2bec6c308bf4382b3062407ff/src/cascadia/TerminalSettingsEditor/Utils.h#L6)
includes `SettingContainer.h` back. In all 149 lines of `Utils.h`, `SettingContainer` appears
only on that `#include` line:

```
$ grep -in settingcontainer src/cascadia/TerminalSettingsEditor/Utils.h
6:#include "SettingContainer.h"
```

The macros in `Utils.h` are text substitution; they need no type until they expand, and by then
the call site already has `SettingContainer.h` in scope. The include looks like a leftover from
a refactor. Deleting it is a one-line change with no call-site impact.

**Counter-argument.** This is a WinRT/XAML settings module leaning on macro boilerplate, where
"who includes whom" is harder to track; a maintainer could reasonably point at the
macro style rather than this one include. It is one cycle in one pair, not a claim about the
whole ~500K-line codebase (archcheck reports 2 SF.9 cycles in that checkout).
