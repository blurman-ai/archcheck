# Literature survey: architectural drift from AI coding agents (2026-07-11)

Four parallel web sweeps (academic / C++-specific / datasets & detection / grey literature),
~50 distinct searches, full-text fetches of the load-bearing papers. Key artifact URLs
spot-checked live (HTTP 200) on 2026-07-11. Purpose: find prior work on agent-caused
architectural drift, especially in C++, and any published repo lists or detection methods
usable for task #180 and beyond.

**Headline: the C++ niche is confirmed empty, and three published repo lists + a validated
detection census now exist that we can join against a C++ language filter.**

## 1. Directly actionable: published repo lists / replication packages

| Artifact | Size / cut | What it gives us | URL |
|---|---|---|---|
| Agarwal, He, Vasilescu (CMU), MSR 2026 — "AI IDEs or Autonomous Agents?" | 401 agent-first repos + 606 controls; 117 IDE-first + 73 controls | Repo lists in replication package; staggered DiD design (closest sibling to #180) | arXiv:2601.13597; github.com/shyamagarwal13/agentic-coding-impact |
| Liu, Widyasari, ..., David Lo — "Debt Behind the AI Boom" | 302.6k verified AI commits, 6,299 repos, 5 assistants | Dataset + attribution recipe (99% validated precision); Python/JS/TS only, but case list includes librealsense (C++) — check their corpus for C++ | arXiv:2603.28592; github.com/yueyueL/tech-debt-ai-coding |
| AIware 2026 config dataset | 4,741 repos with agent configs (Claude Code, Gemini CLI, Codex CLI, Copilot CLI, Cursor), cut 2026-04, CC-BY | `repos.csv` with full names → trivially joinable with GitHub language stats → fresh agent-adopting C++ candidates | arXiv:2605.08435; zenodo.org/records/19375881 |

## 2. Detection methods beyond our current marker set

- **Khosravani & Mockus, "Detecting AI Coding Agents in Open Source: A Validated Multi-Method
  Census of 180M Repositories"** (arXiv:2606.24429). World of Code, Dec 2024–Apr 2026. Four
  validated signal types: bot accounts (100% precision), commit-message signatures (Replit,
  Codex `codex-cli`, 90.5%), author-name suffixes (`(aider)`, 86.8%), config files incl.
  `.windsurfrules`, `copilot-instructions.md` (92%). 12 agents. Key number: **bot-account
  lookup alone undercounts Claude Code ~30×** — validates our multi-signal approach. AI commits
  1.6% (Dec 2025) → 6.7% (Mar 2026) of non-bot activity. Regexes are in the HTML; Zenodo
  replication package promised, not yet posted. No per-language breakdown → "how many
  agent-heavy C++ repos exist" is open and answerable with their method.
- **Robbes et al.: "Agentic Much?"** (arXiv:2601.18341) + **"Promises, Perils, and (Timely)
  Heuristics"** (arXiv:2601.18345, MSR 2026 Distinguished Paper) + follow-up (arXiv:2606.07448).
  150 heuristics (86 file-based, 20 branch, 44 label) for ~50 agents; 129,134 projects; the
  Perils paper catalogs 8 pitfalls of these heuristics — read before extending our detector.
- **Behavioral fingerprinting** (arXiv:2601.17406, MSR 2026): 41 features, 97.2% F1 — but
  attribution *among known agents* on AIDev, not open-set detection. Feature set reusable.

## 3. Closest competitor results (what we must engage with)

- **Agarwal/He/Vasilescu (MSR 2026)**: agent-first repos +36% commits, +77% lines, ~18% more
  SonarQube warnings, +35% cognitive complexity (β=0.299, SE 0.059). **Caveat verified against
  the full text (2026-07-11): cognitive complexity is aggregated at repo-month level with NO
  volume normalization** — on +77% lines, +35% total complexity is the volume channel, not a
  per-unit quality decline (consistent with our P2 per-week ×1.76 at per-commit ≈1.0). Their
  only normalized quality metric, duplication *density*, is +7.9% and not significant —
  matching our per-commit clone null. They also report significant pre-treatment coefficients
  in complexity and acknowledge it as a design limitation (same disease as our P2 pre-trend).
  Net: an external replication of our volume-story, not a contradiction of our nulls.
- **Larsen & Moghaddam** (arXiv:2606.13298, SEAA 2026, already known): Java + ARCAN smells;
  still the only causal architecture-smell study; zero citations yet (too fresh).
- **Mao et al.** (arXiv:2603.27130): **the only paper with C++ content** (7.8% of files);
  finds AI code has *lower* cross-file duplication (17.2% vs 24.5%) — a counterpoint to
  GitClear that we should reconcile (consistent with our own per-commit clone null).
- **Borg et al., "Echoes of AI"** (arXiv:2507.00788, EMSE 2026): controlled 2-phase experiment,
  151 devs, Java: **no measurable maintainability penalty** downstream. Strongest null in the
  space — must be cited and engaged, not ignored.
- **Horikawa et al., "Agentic Refactoring"** (arXiv:2511.04824, TOSEM): agent refactoring is
  low-level (renames); **rarely targets duplication or dependency management** — mechanistic
  support for "agents don't do design-level maintenance" (aligns with our P4 cleanup framing).
- **Sawada et al.** (arXiv:2605.06464, EASE 2026): AI files get less maintenance; AI updates
  are feature extensions, human updates are bug fixes.
- **SmellBench** (arXiv:2605.07001): agents repair only 47.7% of architectural smells; the
  most aggressive agent *introduces* 140 new ones. Also: 63.1% of architecture-smell detector
  output is FP — a caution for ARCAN-style pipelines.
- **SlopCodeBench** (arXiv:2603.24755): structural erosion in 77% of long-horizon agent
  trajectories; **supports a C++ track that nobody has run** — a possible cheap novel result.
- **Konrad et al., "Architecture Without Architects"** (arXiv:2604.04990): coins "vibe
  architecting"; positioning citation for the archcheck pitch, not empirical.
- **Ehsani et al. MLR** (arXiv:2606.14796, TOSEM, w/ Yuanfang Cai): 104-source review of
  LLM-era technical debt; best snowballing hub; notes no standardized LLM-specific debt
  metrics exist.

## 4. Grey literature (fresh numbers beyond GitClear 2025)

- **GitClear × GitKraken 2026** (via LeadDev, 2026-07-07): 623M lines 2023–2026 — duplication
  +81%, moved lines −70%, legacy refactoring −74%, **functional connectivity −35%** (new code
  calling existing functions — nearest grey-lit proxy for "non-integration with existing
  structure"), error masking +47%.
- **Faros AI "Acceleration Whiplash"** (2026-03): 22k devs telemetry — tasks +34% but bugs/dev
  +54%, incidents-per-PR +242.7%, review time ×5. (Often misattributed to DORA — cite Faros.)
- **CodeScene 2026**: unguided Claude Code does shallow refactorings (54k renames vs 7.5k
  extract-methods); agents burn up to 50% more tokens on unhealthy code (reverse causality:
  bad structure degrades agents).
- **Sonar** shipped architecture controls in SonarQube (Dec 2025) explicitly citing AI
  blindness to architectural constraints — the "architecture drift under AI" product category
  now exists; DORA 2025: AI amplifies delivery instability.
- **HackerNoon, 49 vibe-coded repos** (May 2026): jscpd avg 7.98% duplication vs 3–5%
  benchmark; agent skill libraries up to 37%; raw data published — closest grey-lit analog to
  our methodology.
- **DX (Abi Noda)**: maintainability scores commonly decline with AI use; "reduces
  understanding while increasing output".

## 5. Confirmed empty niches (our claims to own)

1. **C++ × agents × architecture: nothing.** Only arXiv:2603.27130 even contains C++ files.
   No study touches include graphs, header hygiene, levelization, physical design, Lakos.
   CppCon/ACCU 2025-26 talks are all "how to use agents on C++", none measure damage.
   PVS-Studio/JetBrains have not analyzed AI-written C++.
2. **Boolean-flag / flag-argument proliferation: unmeasured anywhere** (academic and grey).
   Our `n_bool_field_added` / #119 coupling result appears to be unpublished territory.
3. **Dependency cycles / levelization in AI-adoption studies: nobody measures them.** Closest
   are cognitive complexity + duplication density (2601.13597) and ARCAN smells (2606.13298).
4. **Named heavily-agent-written OSS C++ projects: none surfaced.** The flagship agent-built
   systems project (Anthropic's ccc compiler, 16 agents, ~180k lines) is Rust.

## 6. Implications for task #180 and beyond

- **External anchors now exist for our endpoints**: P1 volume (+36% commits in 2601.13597,
  NBER +109%; their un-normalized +35% total complexity is also a volume-channel datapoint),
  P4 stock (GitClear duplication +81%, agentic-refactoring "agents don't consolidate").
  P2 per-unit complexity has no clean external confirmation — 2601.13597 never normalizes.
  P3 (flags) has *no* external anchor — it is our novel claim.
- **Cheap corpus extension**: join Zenodo 19375881 (4,741 repos) + Liu/Lo 6,299 repos +
  Agarwal 401 agent-first repos against a C++ language filter → candidate agent-heavy C++
  repos beyond our screen_ledger funnel. Also adopt Type B/C regexes (Replit/Codex/Aider)
  from the census to widen marker coverage.
- **Positioning**: cite Borg's null and Mao's lower-duplication finding honestly; our own
  per-commit nulls agree with them — the drift story is volume + stock + review-filtering
  (P1/P4/P5), which none of these papers measure.
