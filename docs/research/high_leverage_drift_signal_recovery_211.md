# High-Leverage Drift Signal Recovery (Task #211)

**Date:** 2026-11-22 · **Task:** [`#211`](../../backlog/wip/211_maj_high_leverage_drift_signal_recovery.md) · **Predecessor:** [`why_drift_looks_null_analysis.md`](why_drift_looks_null_analysis.md)

## Executive summary

1. **Pure-agent stratum (`S1`, ≥90% AI post-adoption) & dose-response (Step 2A):**
   Across all 5 AI-share strata (`Control 0%` → `<10%` → `10–50%` → `50–90%` → `S1 ≥90%`),
   cognitive complexity (`P2_complexity`) rises **monotonically** from **0.116 → 0.358 per commit
   (3.09×)** and **2.02 → 5.34 per KLOC (2.64×)**. Equal-repo weighting confirms the effect
   (**1.73×**, leave-one-out without `derekbsnider_madc`: **1.58×**, repo median **3.44×**).
2. **Collider & task-mix decomposition (Step 2B, 250 mixed adopter repos):**
   Agents touch C/C++ files more frequently than unmarked commits (`74.5%` vs `66.5%`) and write
   **1.37× larger C++ commits** (median `35` vs `23` LOC). Moving from `Raw (all)` → `C++ only,
   unmatched` → `C++ only, LOC-matched` attenuates the within-repo continuous ratio step-by-step
   (`P1_union`: `1.37× → 1.17× → 1.09×`; `P2_complexity`: `1.32× → 1.15× → 1.03×`), directly
   quantifying both the task-mix confound and the LOC-bloat collider.
3. **Cumulative `per-struct boolean drift` on the 854/517 adoption panel (Step 1B):**
   Aggregating `DRIFT.BOOL_FIELD_ACCRETION` events across all `2,692` bool-accreting sampled commits
   separately over `pre` (`[-12w, 0)`) and `post` (`[0, +12w]`) yields consistent adopter-vs-placebo
   DiD ratios across all accumulation thresholds:
   - `≥2` distinct commits/struct in 12w: content structs **1.56×** (`73→92` vs `26→21`), affected repos **1.80×** (`44→62` vs `23→18`).
   - `≥3` distinct commits/struct in 12w: content structs **1.71×** (`21→27` vs `4→3`), affected repos **3.20×** (`15→24` vs `4→2`).
   - `≥4` distinct commits/struct in 12w: content structs **6.00×** (`4→8` vs `3→1`), affected repos **7.00×** (`3→7` vs `3→1`, small-$n$ caveat).
4. **Rename-canonicalized graph diff & `GATE-1` SCC identity in `archcheck` (Steps 3A–3C):**
   - Implemented `git::collectRenameMap` (`git diff --find-renames --diff-filter=R --name-status -z`),
     threaded `RenameMap` through `graph::addedEdges`, `graph::removedEdges`, `graph::grownSccs`,
     `detectNewCrossAreaDependencies`, and `detectNewGodHeaders` (`GRAPH-2`), and updated
     `graph::grownSccs` to membership-subset semantics (`GATE-1`, catching `{a,b} → {a,c}`).
   - Verified empirically on real corpus `rename_bulk` commits: **100.0% reduction** in spurious
     `added_edges` (`8,216 → 0` across 7 tested top commits, including `picolibc b4faae61ae`
     `2,353 → 0` and `MaSzyna 0531086bb9` `1,045 → 0` edges / `165 → 0` cross-area deps).
   - **Important reversal in Step 3C:** Commits with `≥1` exact C/C++ rename accounted for **30.8%**
     (`47,498 / 154,337`) of all `added_edges` in `results_window`. Stripping them *reduces*
     `S_added_edges` DiD from `2.21×` → **`1.05×` per commit** (`0.87×` per KLOC; `1.61×` per week),
     showing rename noise was *inflating* raw `added_edges` rather than masking it. Meanwhile
     `P1_union` remains robust at **`2.23×/week`, `1.46×/commit`, `1.20×/KLOC`**.
5. **Window-edge `--drift-baseline` (`DRIFT.1`–`DRIFT.4`, Step 1A — COMPLETE, 1,376 repos):**
   - `1,258` OK repos (`802` adopters / `456` controls; `1,197` with zero missing blobs: `758` / `439`).
   - **`DRIFT.1` (shortcut edges across architectural layers)** rises significantly per 12-week window:
     - Equal-repo-weighted `log1p(post) - log1p(pre)` DiD (immune to whale repos): **`1.234×` (95% CI `[1.071, 1.428]`)**.
     - Winsorized (`cap=500` edges/repo): **`1.34×` per window** (`0.67×` per KLOC, reflecting the `1.94×` adopter/control volume shift).
     - Raw pooled: `3.95×` per window (`2.03×` per KLOC), heavily influenced by `Open-Cascade-SAS_OCCT` (`9,090` ctrl pre) and `openvino` (`6,217` adopt post) — hence the winsorized and `log1p` estimates are the authoritative numbers.
     - In `S1` (≥90% AI, `n=11` OK repos), `DRIFT.1` rises `32 → 49` findings and affected repos **double (`4/11 → 8/11`)**.
   - **`DRIFT.2`, `DRIFT.3`, `DRIFT.4.*`:** Repo-level prevalence leans positive for `DRIFT.3` (`1.40×`), `DRIFT.4.NEW` (`1.35×`), and `DRIFT.4.CYCLE` (`1.28×`), but `log1p` DiD is near null (`0.98×–1.01×`, all 95% CIs span `1.0`) due to rarity at 12-week horizons.

---

## Step 2A: Dose-response across AI-share strata

Script: `experiments/signal_recovery_211/step2_unattenuated_agent_effect.py` · Data: `experiments/signal_recovery_211/step2_results.json`.

| Metric | Control (`0%`, n=504) | `<10%` AI (n=597) | `10–50%` AI (n=163) | `50–90%` AI (n=70) | `S1 ≥90%` AI (n=15) | `S1 / Ctrl` (commit / KLOC) |
|---|---|---|---|---|---|---|
| `P1_no_edges` | 0.756 / 13.19 | 1.052 / 15.55 | 1.506 / 18.09 | 1.829 / 16.92 | **1.341 / 20.00** | **1.77× / 1.52×** |
| `P2_complexity` | 0.116 / 2.02 | 0.164 / 2.42 | 0.214 / 2.56 | 0.326 / 3.01 | **0.358 / 5.34** | **3.09× / 2.64×** |
| `P3_flag` | 0.038 / 0.67 | 0.056 / 0.83 | 0.085 / 1.03 | 0.090 / 0.83 | 0.051 / 0.77 | 1.34× / 1.15× |
| `P4i_newclone` | 0.547 / 9.55 | 0.741 / 10.95 | 1.154 / 13.85 | 1.325 / 12.26 | **0.915 / 13.65** | **1.67× / 1.43×** |
| `S_added_edges` | 0.471 / 8.22 | 0.886 / 13.09 | 0.995 / 11.94 | 0.586 / 5.42 | 0.302 / 4.51 | 0.64× / 0.55× |
| `S_satd` | 0.055 / 0.95 | 0.091 / 1.35 | 0.053 / 0.64 | 0.089 / 0.82 | 0.016 / 0.24 | 0.30× / 0.26× |

Equal-repo-weighted sensitivity check (`S1` n=15 vs pure-human `Control` n=480 with post commits):
- `P2_complexity`: repo-mean `0.2357` (LOO without `derekbsnider_madc`: `0.2151`) vs `0.1364` → **1.73×** (LOO **1.58×**); repo median `0.2131` vs `0.0620` (**3.44×**).
- `P1_no_edges`: repo median `0.7000` vs `0.3309` (**2.12×**); repo mean `1.046` vs `1.068` (`0.98×`, control mean pulled up by extreme outlier repos).

---

## Step 2B: Mixed-repository collider & task-mix decomposition

250 mixed adopter repositories with `≥3` agent and `≥3` human C++ post-adoption commits (`status == ok`).
- C++ touch share: agent **74.5%** vs human **66.5%**.
- Median C++ commit size: agent **35 LOC** vs human **23 LOC** (median within-repo ratio **1.37×**).

| Metric | 1. Raw all (`cont / bin`) | 2. C++ unmatched (`cont / bin`) | 3. C++ LOC-matched (`cont / bin`) |
|---|---|---|---|
| `P1_union` | `1.370× / 1.248×` | `1.173× / 1.134×` | `1.090× / 1.079×` |
| `P1_no_edges` | `1.317× / 1.281×` | `1.120× / 1.155×` | `1.074× / 1.088×` |
| `P2_complexity` | `1.323× / 1.356×` | `1.148× / 1.189×` | `1.032× / 1.135×` |
| `P4i_newclone` | `1.342× / 1.234×` | `1.131× / 1.118×` | `1.090× / 1.002×` |
| `S_added_edges` | `1.502× / 1.345×` | `1.318× / 1.219×` | `1.127× / 1.072×` |
| `S_cross_area` | `2.231× / 1.826×` | `2.296× / 1.980×` | `2.108× / 0.968×` |
| `S_god_headers` | `2.235× / 2.023×` | `2.280× / 2.026×` | `3.033× / 2.456×` |

---

## Step 1B: Cumulative `per-struct boolean drift` (`pre` vs `post`, 854/517 panel)

Script: `experiments/signal_recovery_211/step1b_perstruct_bool_drift.py` · Data: `experiments/signal_recovery_211/step1b_results.json`.

| Threshold (distinct commits in 12w) | Metric | Adopter `pre → post` (RR) | Control `pre → post` (RR) | DiD ratio |
|---|---|---|---|---|
| `≥2` commits | `content_structs` | `73 → 92` (`1.26×`) | `26 → 21` (`0.81×`) | **1.56×** |
| `≥2` commits | `content_repos` | `44 → 62` (`1.41×`) | `23 → 18` (`0.78×`) | **1.80×** |
| `≥3` commits | `content_structs` | `21 → 27` (`1.29×`) | `4 → 3` (`0.75×`) | **1.71×** |
| `≥3` commits | `content_repos` | `15 → 24` (`1.60×`) | `4 → 2` (`0.50×`) | **3.20×** |
| `≥4` commits | `content_structs` | `4 → 8` (`2.00×`) | `3 → 1` (`0.33×`) | **6.00×** |
| `≥4` commits | `content_repos` | `3 → 7` (`2.33×`) | `3 → 1` (`0.33×`) | **7.00×** |

Top verified `adopter_post` example: `joshualongenecker_home-assistant-bridge-esphome` (`S1`, 90.1% AI),
`struct GeappliancesBridge`: 6/6 AI commits added 7 boolean state flags in 12 weeks.

---

## Step 3C: Rename noise audit on `results_window`

Script: `experiments/signal_recovery_211/step3c_rescore_renames.py`.

| Regime | `P1_union` (wk / commit / KLOC) | `S_added_edges` (wk / commit / KLOC) | `graph_drift` (wk / commit / KLOC) |
|---|---|---|---|
| `A. with_renames` (all `154,337` edges) | `2.89× / 1.88× / 1.53×` | `3.40× / 2.21× / 1.80×` | `3.39× / 2.21× / 1.80×` |
| `B. nov6_rename_bulk_excl` (`-12.6%` edges) | `2.59× / 1.69× / 1.37×` | `2.56× / 1.67× / 1.36×` | `2.55× / 1.66× / 1.35×` |
| `C. zero_exact_renames` (`-30.8%` edges) | **`2.23× / 1.46× / 1.20×`** | **`1.61× / 1.05× / 0.87×`** | **`1.61× / 1.05× / 0.87×`** |

Empirical replay of the new rename-canonicalized `build/debug/src/archcheck --diff` on top `rename_bulk` commits:
- `picolibc b4faae61ae` (2,245 renames): `2,353 → 0` edges, `4 → 0` cross-area
- `Samsung_ONE 6ea42f45e0` (518 renames): `1,231 → 0` edges
- `QDU-VRobot_AUTO-Aming-system b9239f9d46` (1,598 renames): `1,050 → 0` edges, `1 → 0` cross-area
- `MaSzyna-EU07_maszyna 0531086bb9` (220 renames): `1,045 → 0` edges, `165 → 0` cross-area
- `ne-app-eu_krnl 83d870e584` (301 renames): `923 → 0` edges, `3 → 0` cross-area
- `Samsung_ONE 390c183d7b` (354 renames): `893 → 0` edges
- `ZeroFxc_LxcLuaCore c8b27382af` (150 renames): `721 → 0` edges, `4 → 0` cross-area
- **Total across 7 commits: `8,216 → 0` added edges (`-100.0%`), `177 → 0` cross-area deps (`-100.0%`).**

---

## Step 1A: Full 1,376-repo window-edge `--drift-baseline` (`DRIFT.1`–`DRIFT.4`)

Script: `experiments/signal_recovery_211/step1a_drift_baseline.py` · Analyzer: `experiments/signal_recovery_211/analyze_step1a.py` · Data: `experiments/signal_recovery_211/step1a_full.jsonl` (`1,258` OK repos: `802` adopt / `456` ctrl; zero-missing-blob subset: `758` adopt / `439` ctrl).

| Rule | Pooled `Adopt` pre→post | Pooled `Ctrl` pre→post | Repo prevalence DiD | `log1p` DiD (zero-missing-blob, 95% CI) |
|---|---|---|---|---|
| `DRIFT.1` | `21,000 → 30,137` (`1.44×`) | `15,125 → 5,500` (`0.36×`) | **1.105×** (`513→555` vs `291→285`) | **`1.234×` `[1.071, 1.428]`** |
| `DRIFT.2` | `233 → 148` (`0.64×`) | `26 → 23` (`0.88×`) | `0.839×` (`62→52` vs `20→20`) | `0.979×` `[0.949, 1.008]` |
| `DRIFT.3` | `170 → 194` (`1.14×`) | `64 → 44` (`0.69×`) | **1.395×** (`73→70` vs `32→22`) | `1.012×` `[0.976, 1.050]` |
| `DRIFT.4.NEW` | `66 → 56` (`0.85×`) | `31 → 38` (`1.23×`) | **1.346×** (`32→35` vs `16→13`) | `0.998×` `[0.970, 1.027]` |
| `DRIFT.4.CYCLE` | `17 → 21` (`1.24×`) | `7 → 6` (`0.86×`) | **1.275×** (`16→17` vs `6→5`) | `1.004×` `[0.988, 1.021]` |
| `DRIFT.4.SDP` | `8 → 16` (`2.00×`) | `2 → 13` (`6.50×`) | `0.667×` (`7→14` vs `2→6`) | `0.997×` `[0.983, 1.010]` |

Whale sensitivity for `DRIFT.1` (zero-missing-blob subset, `758` adopt / `439` ctrl):
- Uncapped pooled: `DiD/win = 4.16×`, `DiD/KLOC = 2.08×` (driven by `Open-Cascade-SAS_OCCT` `9,090` ctrl pre and `openvino` `6,217` adopt post).
- Winsorized (`cap=500`): `adopt 16,876 → 19,334 (1.15×)` vs `ctrl 6,134 → 5,237 (0.85×)` → **`DiD/win = 1.34×`**, `DiD/KLOC = 0.67×`.
- Winsorized (`cap=100`): `adopt 11,821 → 12,875 (1.09×)` vs `ctrl 5,087 → 4,823 (0.95×)` → **`DiD/win = 1.15×`**, `DiD/KLOC = 0.57×`.
- `NCCD delta`: adopters `>0` rises `408/802 → 427/802` (`50.9% → 53.2%`, median `+0.0004 → +0.0010`), controls `>0` falls `222/456 → 209/456` (`48.7% → 45.8%`, median `0.0000 → 0.0000`).
