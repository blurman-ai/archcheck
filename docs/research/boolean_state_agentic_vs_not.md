# Boolean-State Drift: agentic vs ordinary repos (by commits)

**Date:** 2026-06-07 · **Task:** #089
**Method:** per-struct drift over git history (depth-0 fields → git blame → grouping by struct; drift = bool fields of one content struct arrived through ≥4 distinct commits). Run over the **787 repos** of the corpus, tagged agentic (73) / non-agentic (714).

## Result

| Group | Repos | Candidate structs (≥4 bool) | Drift (content) | **drift / examined** | Repos with drift |
|---|---|---|---|---|---|
| **AGENTIC** | 73 | 518 | 51 | **9.8%** | 27/73 (**36%**) |
| **NON-AGENTIC** | 714 | 5046 | 304 | **6.0%** | 126/714 (**17%**) |

**Agentic repos drift on bools noticeably more often:**
- per struct: **9.8% vs 6.0% ≈ 1.6×**
- per repo: **36% vs 17% ≈ 2.1×**

## Confound checked — this is NOT "agentic repos are more active"

The natural objection: agentic repos were selected for high activity (>300 commits since May 2025), and more commits in the window = more chances to accumulate ≥4 add-commits. I checked the median (shallow) clone depth:

| | median commits | mean |
|---|---|---|
| AGENTIC | 1013 | 1659 |
| NON-AGENTIC | 940 | 1505 |

The ratio of medians is **1.08×**, i.e. activity is practically equal. The difference in drift (1.6-2.1×) **cannot be explained** by commit volume. The signal holds.

## What it means

Consistent with the project's main theme (the agentic tail, `experiments/AGENTIC_CODE_REPORT.md`): code written with AI assistants **accumulates boolean-state within a single struct noticeably more often** — exactly the "add one more flag for the new feature" pattern that agents reproduce more eagerly (each iteration = a new bool, without refactoring into enum/State).

## Verification of the config-bag caveat (eye-check of both groups)

I ran a manual classification of samples from both groups (rubric 🟢 real drift / 🟡 config-bag / 🔴 FP):

| Group | sample | 🟢 real | 🟡 config-bag | 🔴 FP |
|---|---|---|---|---|
| AGENTIC | 14 | **57%** | 43% | **0%** |
| NON-AGENTIC | 15 | **40%** | 40% | **20%** |

The config-bag share is comparable (~40-43%), but the **FP share is higher for non-agentic** (20% vs 0%): their top sample included giant vendor headers (hostap `wpa_supplicant`, freebsd `amdgpu_device`) and generated code (EVerest `Conf`) — agentic repos have fewer of those (repos are younger/smaller).

**Correction to real drift (drift/examined × 🟢-share):**

| Group | flagged | × 🟢-share | = real drift |
|---|---|---|---|
| AGENTIC | 9.8% | 57% | **5.6%** |
| NON-AGENTIC | 6.0% | 40% | **2.4%** |

→ ratio **grows from 1.6× to ~2.3×**. Caveat #3 is not just removed — the correction **strengthens** the signal (non-agentic has more FP dilution, so its real drift falls more).

## Honest caveats

1. **Both shallow** → lower bound for both groups (same bias, ratio is fair; absolute is understated).
2. **n=73 agentic is modest.** 51 drift structs; per-repo 36% over 73 repos → confidence interval wider than for non-agentic (714).
3. ~~I assume the config-bag share is comparable in both groups~~ → **CHECKED** (see section above): config-bag is comparable, FP is higher for non-agentic; the correction strengthens the ratio to ~2.3×. The absolute 9.8/6.0 are "candidates"; confirmed drift ≈ 5.6% vs 2.4%.
4. **Does not prove causation** AI→drift; it shows correlation while controlling for activity.

## Conclusion

At equal commit activity, agentic C++ repos show more boolean-state drift: **~1.6× on raw flags, ~2.3× after correction for verified real drift** (5.6% vs 2.4% of structs). The signal is robust to two checks — the activity confound (1.08×, removed) and FP/config composition (eye-check of both groups, ratio strengthened). Unlike a static counter, it is temporal. A standalone normalized contribution to the "agentic tail". Data: `experiments/boolean_state/perstruct_drift_all.csv`.
