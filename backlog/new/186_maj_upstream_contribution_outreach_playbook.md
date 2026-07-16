# [RELEASE][DOCS] 186: upstream-contribution outreach — turn findings into genuine PRs/issues, not spam

**Created:** 2026-07-14
**Start date:** —
**Status:** new
**Module:** RELEASE / DOCS
**Priority:** major
**Difficulty:** medium (mostly judgement + drafting; low code)
**Blocks:** —
**Blocked by:** — (evidence base #160/#161/#162 done; showcase 010–012 done)
**Related:** #163 (public launch plan — this is its Phase-4 "serial findings posts /
upstream issues" item, operationalized), #160/#161/#162 (hand-verified evidence base),
`experiments/showcase/010–012` (the first fuel), archcheck-action (the CI-adoption vehicle)

## Goal

Build a **repeatable, honest playbook** for using real archcheck findings on well-known
C++ OSS projects as a growth channel — via genuine upstream contributions and public
write-ups — **without** falling into the drive-by "here's my tool, please adopt it" PR
spam pattern that damages the product's name.

The naive framing this task exists to correct: *"open a PR with a fix on a big mature
repo and write there: here's my archcheck, I advise you adopt it."* That literal version
is a textbook self-promo anti-pattern and must NOT be done — see "Do NOT do".

## Why the literal version backfires (the trap)

1. **Drive-by promo PRs read as spam.** Maintainers close them; worse, they get
   screenshotted and mocked, and that sticks to the *product name*, not the account.
2. **AI-PR backlash (2024–2026).** Many projects now scrutinize or ban AI-generated PRs
   (the same sensitivity that got r/cpp dropped from our launch, #163, over its AI-content
   ban). archcheck is AI-built → double the scrutiny.
3. **Architectural findings are a poor PR fit.** archcheck's flagship outputs (SF.9
   cycles, god-headers, chain length, drift) are large, risky refactors — no maintainer
   merges a cycle-break from a stranger. Only a narrow subset (missing include guard,
   a genuinely safe small include cleanup) is PR-able at all.

## The two legitimate models (both already partly built here)

**Model A — "we ran the tool on your famous project" write-up** (PVS-Studio / Coverity
Scan / scc playbook). Scan a known repo → hand-verify a real finding → publish a write-up
with methodology + `file:line` + honest caveats. The tool is credited **in the write-up**,
not injected into the maintainer's queue. This is exactly `experiments/showcase/` and the
#163 Phase-4 "serial archcheck vs <project> findings posts" item. Highest leverage, lowest
risk, content compounds.

**Model B — "contribute first, mention modestly."** Find a **real, small, self-contained
fix** that stands on its own merits (mergeable even if the tool never existed). The PR is
**about the fix**. One factual sentence: *"Found via archcheck (my open-source C++
architecture linter): <link>"* + affiliation disclosed. No "please adopt", no CI ask
bundled in. Never make the fix conditional on tool adoption.

**The CI-adoption ask lands last and soft.** Only after credibility exists (stars / a
merged fix / an accepted issue), as an opt-in offer (Discussion or a follow-up issue)
pointing at the 5-line archcheck-action — never as a demand inside a PR.

**Routing rule:** most archcheck findings → a well-researched **issue / write-up**, NOT a
PR. A PR only for the narrow safe-fix subset.

## Scale & ethics guardrails (non-negotiable)

- **Hand-crafted, 3–5 carefully chosen repos.** Not a bot, not a mass campaign, not
  automation. (Also our own constraint against mass-targeting.)
- **Disclose affiliation every time.** No sockpuppets, no astroturf, no friend-boosting.
- **Respect each repo's CONTRIBUTING / CoC / AI-content policy.** Where AI PRs are banned:
  skip, or contribute only human-written text.
- **Every finding hand-verified in both directions from git objects** (the showcase bar).
  A wrong finding attached to the product name is worse than no outreach.
- **The user submits.** Announcements/contributions are outward-facing (same rule as #163
  Phase 3). The agent prepares fuel + drafts + verification; the user posts in their own
  words.

## Plan

### Phase 1 — Curate fuel (agent)
- Inventory hand-verified findings we already have (`experiments/showcase/001–012`) + run
  fresh zero-config scans on a shortlist of well-known, welcoming C++ repos.
- For each finding, **classify the vehicle**: `PR-able small fix` | `issue/report`
  | `write-up only`. Most cycles/god-headers → issue or write-up.
- Record in a **target ledger** (companion `launch_drafts/`, not public backlog): repo,
  finding, `file:line`, verified?(both directions), vehicle, repo's AI-PR policy, status.

### Phase 2 — Draft the vehicles (agent → user edits)
- **PR description template** (Model B): fix-first, one modest tool line, affiliation
  disclosure, links CONTRIBUTING compliance. Human-editable, non-slop.
- **Issue/Discussion template** (Model A findings that aren't safely fixable): the finding,
  methodology, `file:line`, honest counter-argument, reproduce steps — no pitch.
- Keep both in the companion `launch_drafts/outreach/`.

### Phase 3 — The soft CI-adoption offer (later)
- A short, reusable "want this as a CI check?" note pointing at archcheck-action, deployed
  ONLY on a signal of interest or after a merged fix — never attached to the initial PR.

### Phase 4 — Compounding write-ups (feeds #163 Phase 4)
- Turn each verified finding into an "archcheck vs <project>" write-up (the higher-leverage,
  lower-risk asset). This is where the tool name actually gets sold.

## Deliverables

| Artifact | Location |
|---|---|
| Outreach playbook (this task's models + guardrails, distilled) | companion `launch_drafts/outreach/playbook.md` |
| PR / issue / CI-offer templates | companion `launch_drafts/outreach/` |
| Target + finding ledger (repo, finding, vehicle, verified, status) | companion `launch_drafts/outreach/ledger.tsv` |
| This task | `backlog/new/186_maj_upstream_contribution_outreach_playbook.md` |

(Target lists and outreach drafts go to the PRIVATE companion, like the rest of the
launch kit — the public backlog stays clean, per #167.)

## Do NOT do

- **No drive-by "use my tool" PRs.** No PR whose primary purpose is promoting archcheck.
- **No mass automation / bot-submitted PRs / templated blasts.** 3–5 hand-picked, max.
- **No fix conditional on tool adoption**, and no CI-integration ask inside a fix PR.
- **No undisclosed affiliation, no sockpuppets, no friend-boosting.**
- **No unverified finding** goes out under the product name.
- **Do not submit anything from the agent** — the user is the one who contributes/posts.

## Acceptance

- [ ] Playbook + templates written and user-reviewed (companion `launch_drafts/outreach/`).
- [ ] Target ledger populated with ≥3 hand-verified findings, each with a chosen vehicle.
- [ ] For each of ≥3 repos: a ready-to-send draft (PR or issue) the user can post as-is or edit.
- [ ] Each repo's AI-PR / contribution policy checked and recorded before anything is sent.
- [ ] Zero drive-by promo PRs; every vehicle stands on its own merit.
