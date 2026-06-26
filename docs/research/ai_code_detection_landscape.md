# AI code in repositories: detection methods, datasets, measurements

_A reference on external research (state of the field). Compiled in the session of
2026-05-30 while working through the question "how to recognize an AI repository and where to look for
architectural drift"._

**Purpose of the file:** record facts, methods, and figures from others' work —
without project conclusions. Interpretation and decisions for archcheck are deferred to
a separate task (see §"Where this is going" at the end). Here, only what others
measured.

Related to: [constraint_decay.md](constraint_decay.md) (the project's root cause),
[ai_drift_runlog.md](ai_drift_runlog.md) (our own DRIFT run over
33 PRs), [backlog/future/033](../../backlog/future/033_maj_ai_drift_dataset.md)
(AI-drift dataset).

---

## 1. How AI code is recognized in a repository — three classes of methods

In decreasing order of precision and increasing order of cost.

### 1.1. Explicit git metadata (attribution-based)

The simplest and most precise method: AI tools leave traces in git metadata.
Recognized without classifiers, purely by string match.

Signals (from [«Debt Behind the AI Boom»](https://arxiv.org/abs/2603.28592)):

- **actor login** — e.g. `copilot-swe-agent[bot]`, `github-copilot[bot]`
- **author email** — e.g. `noreply@anthropic.com`
- **author name** — e.g. `Cursor Agent`
- **`Co-Authored-By` trailer** in the commit body

The work reports that **29 AI tools** leave recognizable traces in
git metadata.

A similar set of markers at the PR level is systematized in the open-source rule set
[Coderbuds](https://coderbuds.com/blog/open-source-ai-code-detection-yaml-rules):

| Tool | Marker | Confidence |
|------------|--------|-------------|
| Claude Code | footer `[Claude Code](https://claude.com/claude-code)`; `Co-Authored-By: Claude … <noreply@anthropic.com>` | 100% |
| GitHub Copilot | bot author `github-copilot[bot]` (workspace feature) | 100% |
| Cursor | footer `cursor.com`; HTML comment `<!-- Cursor -->` | 90% |
| Cursor | branch name `cursor-` / `cursor/` | 70% |
| OpenAI Codex | branch name `codex-` | 60% |

Coderbuds estimate that ~75% of AI-assisted PRs are caught by explicit markers with
100% confidence; the rest goes into the behavioral tier (see §1.3).

**Method limitation (named in the sources):** "the true volume of AI contribution
is underestimated", because inline suggestions (Copilot without workspace, early Cursor
versions) and disabled attribution leave no traces.

### 1.2. Config artifacts in the tree

Method: search the repository for the configuration files of AI agents. The presence of
a config is a proxy for "the project uses AI".

Source: [«A Dataset of Agentic AI Coding Tool Configurations»](https://arxiv.org/html/2605.08435).

What was searched for (8 configuration mechanisms for 5 tools — Claude Code, GitHub
Copilot, OpenAI Codex, Cursor, Gemini):

- **Context Files** — `CLAUDE.md`, `AGENTS.md`, `.cursorrules`, `GEMINI.md`
  (the most frequent type: 4,463 repos)
- Skills, Subagents, Commands, Rules, Settings
- Hooks (rare: 101 repos), MCP configurations (rare: 124 repos)

Dataset-building pipeline:

1. 187,304 repos from SEART GitHub Search → filter by license/activity →
   40,585 active
2. GPT-5.2 classification "engineered software project" → 36,710 eligible
3. search for config artifacts → **4,738 repos with AI configs**

Measured correlation (key to the method): **71.6% of repositories with a config
(3,392 of 4,738) contain at least one AI-co-authored commit.**

Language composition (top 5 among repos with a config): TypeScript 24.5%, Python 18.7%,
Go 13.8%, Java 8.0%, C# 7.5%.

The dataset is public (15,591 config artifacts, full contents of 18,167 files;
148,519 AI-co-authored commits).

### 1.3. Behavioral fingerprint (for cases without attribution)

Method: an ML classifier over behavioral features of code and commits — works
when there is no attribution at all.

Source: [«Fingerprinting AI Coding Agents on GitHub»](https://arxiv.org/pdf/2601.17406)
(MSR '26).

Features distinguishing the tools (Codex, Copilot, Cursor, Claude Code):
commit-message format, code structure, indentation style, naming
conventions. E.g. Codex — characteristic multiline commits (feature importance
67.5%); Claude Code — code structure, the share of conditional operators (importance
27.2%).

Technique: XGBoost / Random Forest classifiers, SMOTE for class imbalance,
cross-validation. Stated conclusion: AI agents leave recognizable
"fingerprints" even without explicit markers.

Related work on stylometry / authorship attribution of AI code:

- [«I Know Which LLM Wrote Your Code Last Summer»](https://arxiv.org/abs/2506.17323)
  — model attribution for C programs; up to 97.56% accuracy (binary, within a model
  family), up to 95.40% (multi-class).
- [«The Hidden DNA of LLM-Generated JavaScript»](https://arxiv.org/html/2510.10493)
  — structural patterns, 50,000 Node.js programs from 20 LLMs.
- [«Detection of LLM-Generated Java Code Using Discretized Nested Bigrams»](https://arxiv.org/pdf/2502.15740)
  — >96% accuracy on GPT-rewritten Java.

---

## 2. Ready-made datasets (can be taken, not built from scratch)

| Dataset | Volume | Languages | Collection method |
|---------|-------|-------|-------------|
| **AIDev** | 932,791 agentic PR + 6,618 human PR / 116,211 repos | — | agentic PRs from Codex, Copilot, Devin, Cursor, Claude Code |
| **AIDev-pop** | 33,596 agentic PR | — | subset of AIDev |
| **Configs dataset** ([2605.08435](https://arxiv.org/html/2605.08435)) | 4,738 repos / 148,519 AI commits | TS/Py/Go/Java/C# | config artifacts + git markers |
| **Debt Behind the AI Boom** ([2603.28592](https://arxiv.org/abs/2603.28592)) | 304,362 AI commits / 6,275 repos (100+ stars) | Python, JS, TS | explicit git metadata |
| **DevGPT** ([2309.03914](https://arxiv.org/abs/2309.03914)) | 29,778 prompt/response, 19,106 snippets | — | shared ChatGPT conversations ↔ commits/PRs/issues |
| **Security Vulns** ([2510.26103](https://arxiv.org/html/2510.26103v1)) | 7,703 files | — | explicit attribution to 4 tools |

---

## 3. What was measured about the properties of AI code

### 3.1. Where AI code concentrates

[«AI Code in the Wild»](https://arxiv.org/abs/2512.18567) (detection pipeline over the
top-1000 GitHub repos, 2022–2025, + 7000+ CVE-linked changes):

- AI code is a substantial share of new code;
- concentration in **glue code, tests, refactoring, documentation, boilerplate**;
- **core logic and security-critical configs remain predominantly
  human**.

### 3.2. Duplication and erosion of reuse

[GitClear AI Code Quality 2025](https://www.gitclear.com/ai_assistant_code_quality_2025_research)
(211M lines over 5 years; earlier —
[Coding on Copilot](https://www.gitclear.com/coding_on_copilot_data_shows_ais_downward_pressure_on_code_quality),
153M lines 2020–2023):

- blocks of **5+ duplicated lines grew ~8× over 2024**;
- duplication of changed lines: **8.3% (2021) → 12.3% (2024)**, ~4× growth;
- **2024 — the first year copy/paste overtook moved code**;
- refactoring (share of changed lines): **25% (2021) → <10% (2024)**;
- code churn (lines rewritten <2 weeks): **5.5% (2020) → 7.9% (2024)**.

### 3.3. Technical debt and code smells

[«Debt Behind the AI Boom»](https://arxiv.org/abs/2603.28592)
(304k AI commits / 6,275 repos):

- 484,606 distinct issues; **89.1% — code smells** (maintainability),
  5.8% runtime bugs, 5.1% security;
- **>15% of commits from each tool** introduce ≥1 issue (17.3% Copilot …
  28.7% Gemini);
- **24.2% of AI-introduced issues survive** to HEAD;
- humans fix a comparable volume of code smells, but AI introduces **~2× more security
  issues than it fixes**.

[«Security Vulnerabilities in AI-Generated Code»](https://arxiv.org/html/2510.26103v1)
(7,703 files; ChatGPT 91.52% / Copilot 7.50% / CodeWhisperer 0.52% /
Tabnine 0.46%): 4,241 CWE instances, 77 vulnerability types (CodeQL); meanwhile
**87.9% of AI code has no CWE-mapped vulnerabilities**.

### 3.4. AI-PR vs human-PR (behavior)

[«How AI Coding Agents Modify Code»](https://arxiv.org/pdf/2601.17581)
(AIDev: 932,791 agentic + 6,618 human PR): agentic PRs differ substantially in
the number of commits, moderately in the number of touched files and deleted lines; slightly
higher description-to-diff similarity.

[«Programming by Chat»](https://arxiv.org/html/2604.00436v1)
(74,998 messages / 11,579 sessions / 1,300 repos, Cursor + Copilot) — a behavioral
analysis of IDE-native conversational programming.

---

## 4. Other relevant work

- [«A Large-Scale Empirical Study of AI-Generated Code in Real-World Repositories»](https://arxiv.org/abs/2603.27130)
  — detection pipeline (heuristic filter + LLM classifier); properties of code
  and commits (size, post-commit evolution).
- [«On Developers' Self-Declaration of AI-Generated Code»](https://arxiv.org/html/2504.16485v1)
  — 613 self-declared snippets; analysis of self-declaration practices in comments.
- [«Developers and Generative AI: Self-Admitted Usage»](https://arxiv.org/pdf/2603.26277)
  — self-admitted usage in OSS.
- [«AI builds, We Analyze»](https://arxiv.org/html/2601.16839v1)
  — quality of AI-generated build code.
- [How to handle AI-PRs in OSS](https://docs.bswen.com/blog/2026-03-20-ai-generated-pull-requests-opensource/)
  — practical "AI-slop" heuristics (submission speed, duplicate submissions,
  10+ files / <20 lines, absence of tests, symptomatic patching).

---

## Where this is going

This file is only field facts. Project conclusions (what of this applies to
archcheck, how to change the DRIFT-research corpus, the hypothesis about duplication as
a detector) are deferred to a **separate task** and are not mixed in here, so that the
reference stays neutral.

## Sources

- [Debt Behind the AI Boom](https://arxiv.org/abs/2603.28592)
- [A Dataset of Agentic AI Coding Tool Configurations](https://arxiv.org/html/2605.08435)
- [Fingerprinting AI Coding Agents on GitHub](https://arxiv.org/pdf/2601.17406)
- [AI Code in the Wild](https://arxiv.org/abs/2512.18567)
- [A Large-Scale Empirical Study of AI-Generated Code in Real-World Repositories](https://arxiv.org/abs/2603.27130)
- [How AI Coding Agents Modify Code](https://arxiv.org/pdf/2601.17581)
- [Programming by Chat](https://arxiv.org/html/2604.00436v1)
- [On Developers' Self-Declaration of AI-Generated Code](https://arxiv.org/html/2504.16485v1)
- [Developers and Generative AI: Self-Admitted Usage](https://arxiv.org/pdf/2603.26277)
- [Security Vulnerabilities in AI-Generated Code](https://arxiv.org/html/2510.26103v1)
- [AI builds, We Analyze](https://arxiv.org/html/2601.16839v1)
- [I Know Which LLM Wrote Your Code Last Summer](https://arxiv.org/abs/2506.17323)
- [The Hidden DNA of LLM-Generated JavaScript](https://arxiv.org/html/2510.10493)
- [Detection of LLM-Generated Java Code Using Discretized Nested Bigrams](https://arxiv.org/pdf/2502.15740)
- [DevGPT: Studying Developer-ChatGPT Conversations](https://arxiv.org/abs/2309.03914)
- [GitClear AI Code Quality 2025](https://www.gitclear.com/ai_assistant_code_quality_2025_research)
- [GitClear: Coding on Copilot (2023/2024)](https://www.gitclear.com/coding_on_copilot_data_shows_ais_downward_pressure_on_code_quality)
- [Coderbuds: open-source AI code detection rules](https://coderbuds.com/blog/open-source-ai-code-detection-yaml-rules)
- [Handling AI-generated PRs in OSS (BSWEN)](https://docs.bswen.com/blog/2026-03-20-ai-generated-pull-requests-opensource/)
