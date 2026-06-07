# NiCad installation log (Linux / Astra)

_Chronological install + bring-up log for the NiCad evaluation. Goal: get
`nicad6 functions/blocks` running so we can compare its clone detection with
archcheck's duplication pass on the OSS corpus. Companion docs:
[nicad_metrics.csv](nicad_metrics.csv), [nicad_review.md](nicad_review.md),
[nicad_vs_archcheck.md](nicad_vs_archcheck.md)._

## Environment

| Item | Value |
|---|---|
| OS | Astra Linux (Debian-based, `ID_LIKE=debian`) |
| Kernel | 6.1.124-1-generic x86_64 |
| Compiler | gcc (AstraLinux 8.3.0-6+b1) 8.3.0 |
| make / awk / perl | GNU Make 4.2.1 / GNU Awk 4.2.1 / perl 5.28.1 |
| RAM / cores | 31 GiB / 20 |
| Date | 2026-06-07 |

## Versions installed

| Component | Version | Source |
|---|---|---|
| FreeTXL | **10.8b (13.7.22)** | `https://www.txl.ca/download/1148484-txl10.8b.linux64.tar.gz` (527 KB) |
| NiCad | **6.2 (13.11.20)** | `https://www.txl.ca/download/1148644-NiCad-6.2.tar.gz` (1.1 MB) |
| TXL ANSI C++ grammar (tested, not used) | Cpp 3.0, rev 21.3.01 | `https://www.txl.ca/examples/Grammars/Cpp/Cpp.tar.gz` |

Both downloads sit behind a click-through CGI license form (`POST Platform=…&Submit=I Agree`)
rather than a plain URL — scripted with `curl --data 'Platform=linux64&Submit=I Agree'`
against `cgi-bin/txl-download.cgi` / `cgi-bin/txl-nicaddownload.cgi`, which returns the
real tarball link.

## Step 1 — Install TXL

```bash
mkdir -p /home/localadm/opt/src && cd /home/localadm/opt/src
curl -O https://www.txl.ca/download/1148484-txl10.8b.linux64.tar.gz   # via CGI form
tar xzf 1148484-txl10.8b.linux64.tar.gz
cd txl10.8b.linux64
printf 'y\n' | sh ./InstallTxl        # see issue #1
export PATH="$HOME/bin:$PATH"
txl -version   # -> TXL v10.8b (13.7.22)
```

**Issue #1 — `InstallTxl` is interactive and assumes root.** It prompts
`Do you want to continue to install TXL for yourself only? (y/n)` and otherwise
targets `/usr/local`. *Workaround:* pipe `y` in; it then installs user-local to
`~/bin` (binaries `txl txlc txldb txlp`) and `~/txl/lib`, rewriting the lib path in
`txlc`/`txlp` via sed. Self-test (`txl ultimate.question` → `42`) passed.
**Requirement:** `~/bin` must be on `PATH` for everything downstream.

## Step 2 — Build NiCad

```bash
cd /home/localadm/opt/src
curl -O https://www.txl.ca/download/1148644-NiCad-6.2.tar.gz   # via CGI form
tar xzf 1148644-NiCad-6.2.tar.gz
cd NiCad-6.2
export PATH="$HOME/bin:$PATH"
make            # precompiles every *.txl plugin via txlc -s 400
./nicad6        # prints usage -> built OK
```

`make` completed with exit 0, no errors — it `txlc`-compiles ~hundreds of TXL
plugins (`*.x`). NiCad is "install-free for personal use": run `./nicad6` from its
own directory (the launcher hard-codes `LIB=.`). No global install was needed.

NiCad usage confirmed:
```
Usage:  NiCad granularity language systems/systemdir [ config ]
          granularity ∈ { functions blocks ... }
          language    ∈ { c java cs py ... }
          config      ∈ { blindrename ... }
```

## Step 3 — The C++ problem (the decisive finding)

The task calls for `nicad functions cpp` / `nicad blocks cpp`. **NiCad 6.2 has no
`cpp` language**, so that command cannot run. Investigated and confirmed three
independent, compounding blockers:

**Blocker A — no C++ front-end.** Languages actually shipped (from `txl/*.txl`):
`atl, c, cs, java, php, py, rb, rs, sol, swift, wsdl`. There is a `c.grm` but no
`cpp.grm`. `nicad6 functions cpp …` ⇒ `*** ERROR: Language cpp not supported`.

**Blocker B — the `c` extractor only globs `.c`.** `scripts/Extract:114` selects
files with `find … | grep "\.c$"`, i.e. literally files ending in `.c`. C++ files
(`.cpp .cc .cxx .h .hpp`) are invisible to it. Extension census of the corpus shows
this matters — most repos are ~100% C++ with almost no native `.c`:

| repo | .c | .cpp/.cc | .h | .hpp |
|---|---|---|---|---|
| Catch2 | 0 | 226 | 1 | 187 |
| LibreSprite | 2 | 634 | 647 | 0 |
| BambuStudio | 84 | 1256 | 1076 | 757 |
| AetherSDR | **773** | 324 | 483 | 0 |
| GWToolboxpp | 3 | 204 | 350 | 3 |
| Kartend | 0 | 471 | 377 | 0 |
| IrredenEngine | 1 | 190 | 6 | 588 |
| irrlicht-1.8.3 | 132 | 281 | 484 | 0 |

**Blocker C — the C grammar can't parse modern C++.** Even after making the files
visible (see workaround), NiCad's `c` grammar rejects the vast majority of C++:

- Probe, 20 Catch2 C++ files → `17 source files failed to parse`, **0 functions extracted**, renaming aborts (`code 99`).
- Probe, 40 irrlicht (older, procedural) C++ files → `34 failed to parse`, 3 functions (functions) / 18 blocks (blocks), **0 clone pairs** either way.

**Tried: the official TXL ANSI C++ grammar** (`examples/Grammars/Cpp`, "ANSI C++
3.0", last revised 2001 — pre-C++11). It parses its own 1990 `groff.cpp` example
but fails on **7 of 8** modern corpus files (`TXL0192E Syntax error` at the first
modern construct, often the includes). It is also not wired into NiCad — using it
would require authoring the whole `cpp-extract/rename/filter/abstract/normalize ×
functions/blocks/files` TXL program family against its nonterminals. That is a
grammar-engineering project, not an install step.

**Conclusion of Step 3:** out of the box, and with the only available C++ grammar,
NiCad cannot meaningfully process this modern-C++ corpus. NiCad finds clones only
in the subset of files that happen to parse as C (real `.c`, e.g. AetherSDR's 773
C files; vendored C in BambuStudio/irrlicht). This is consistent with NiCad's own
positioning as a research tool (see
[clone_tools_landscape.md](../docs/research/clone_tools_landscape.md): NiCad =
"research (10 MLOC), no CI integration").

## Step 4 — Workaround used for the experiments

To give NiCad the **maximum fair chance** and to *quantify* blocker C corpus-wide,
each repo is run through a "C farm": its directory tree is mirrored with hardlinks
(same filesystem `/dev/nvme1n1p3`, zero extra disk) where every
`.c/.cpp/.cc/.cxx/.h/.hpp/.hxx/.hh` file gets a `.c` suffix so the `c` extractor
sees it. Directory structure is preserved so NiCad's clone reports keep meaningful
paths (needed for the cross-module review). Then:

```bash
cd /home/localadm/opt/src/NiCad-6.2
./nicad6 functions c <farm>     # default config: threshold 0.3, blind rename, cluster
./nicad6 blocks    c <farm>
```

This faithfully stands in for the requested `nicad … cpp`: it presents all C++
source to NiCad, parses what the C grammar can, and records the parse-failure rate
plus whatever clones emerge. Native `.c` files are included, so genuine C clones
(AetherSDR etc.) surface in the same run.

NiCad writes outputs as **siblings of the input dir** (`<farm>_functions.xml`,
`<farm>_functions-clones-<date>/`, HTML/CSV reports, a `…-clones-*.log`). Farms live
under `/home/localadm/nicad_work/` to keep the corpus tree (`/home/localadm/oss`)
clean.

## Issues & workarounds — summary

| # | Issue | Workaround |
|---|---|---|
| 1 | `InstallTxl` interactive + root-oriented | `printf 'y\n' \| sh ./InstallTxl` → user-local `~/bin`, add to PATH |
| 2 | Downloads behind click-through CGI license | scripted `curl --data 'Platform=…&Submit=I Agree'` to the cgi endpoint |
| 3 | `nicad6` launcher hard-codes `LIB=.` | run from the NiCad-6.2 dir (no global install) |
| 4 | **No `cpp` language** | run `c`; the literal `nicad … cpp` is impossible |
| 5 | `c` extractor globs only `.c` | hardlink farm renaming all C++ extensions to `.c` |
| 6 | **C grammar rejects modern C++ (~85–95% files)** | none available; ANSI C++ grammar also fails on modern code → documented as the headline finding |

## Reproducibility

A reader on a Debian-like box can reproduce by running the commands in Steps 1–2
verbatim (TXL first, then `make` NiCad with `~/bin` on PATH), then Step 4 for any
repo. The C++ parse-failure result (Step 3) is the load-bearing finding and is
reproduced by the two probe commands quoted above.
