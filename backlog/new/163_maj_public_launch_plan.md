# [RELEASE] Public launch plan — packaging, hygiene, evidence, announcement

**Created:** 2026-07-02
**Status:** wip — Phase 0 done (9/9), Phase 1 packaging done (Action + Docker shipped, tap
deferred to #166), Phase 2 evidence done (findings + post drafted). Remaining = Phase 3
announcement, which is the user's to execute. **r/cpp dropped from the launch (2026-07-04);
Show HN deferred on launch day (2026-07-07) by HN's temporary Show HN restriction for
new/not-yet-established accounts. Active sequence = isocpp + Meeting C++ → Discord/LinkedIn;
HN becomes a second-chance channel after account warm-up or a later resubmission window.**
Per-venue sign-up guide in companion `launch_drafts/signup_guide.md`. **User registered on
all venues (2026-07-04); launch day set: Tuesday 2026-07-07.** Agent re-verified all kit
links on 2026-07-07: public repo, findings, central finding, demo, Action, isocpp,
Meeting C++, Discord invite, and HN guideline pages all returned HTTP 200. Checkpoint
2026-07-02 below.
**Module:** RELEASE / DOCS / CI
**Priority:** major
**Complexity:** L (multi-phase, mostly S/M items)
**Blocks:** public announcement
**Blocked by:** #160/#161/#162 (evidence base — the "no nonsense goes public" gate) — done;
#127/#131 applicability sign-off (vendored/generated noise tail) — done, residual → #153
**Related:** #138 (release-readiness sync), #142 (prebuilt binary), #156 (demo repo),
#150 (EN translation), #003 (name check)

## Checkpoint — 2026-07-02

### Done (agent-built, hand-verified, shipped)

- **Phase 0 (repo hygiene) — 9/9.** Community files, rename, history scrub (#167), and the
  last open item — a README demo visual: animated `--diff` GIF (`docs/assets/`, verbatim on
  the #156 demo repo).
- **Readiness gate — closed.** Evidence base #160/#161/#162 done; #131 vendored/generated
  sign-off done (→ `completed/`, residual → #153); first-run sanity on 5 famous repos
  (fmt, spdlog, nlohmann/json, rocksdb, terminal) — caught + fixed 2 classification bugs
  (fmt self-exclusion, `dep/` container) and a `.C` extension gap.
- **Phase 1 packaging:**
  - GitHub Action → **public** [archcheck-action](https://github.com/blurman-ai/archcheck-action)
    (`v1`/`v1.0.0`). Composite: pinned+checksummed static binary, `--diff`, step summary,
    sticky comment (github-script, no 3rd-party dep).
  - Docker/GHCR → **in `release.yml`** (`docker` job + `Dockerfile`); alpine (git needed for
    `--diff`), `:latest` on real releases only.
  - Homebrew tap → **deferred to #166** (macOS binary); formula sha256-verified, staged in
    private companion `launch_drafts/homebrew-archcheck-ready/`.
- **Phase 2 evidence:** 3 SF.9 cycle findings in `experiments/showcase/010–012` (folly,
  rocksdb, terminal — both directions verified from git objects, honest counter-arguments);
  ripgrep-shaped launch post drafted in companion `launch_drafts/launch_post_draft.md`.
- **Authorship:** LICENSE copyright set to `Sergey Blekher` across main repo + all artifacts
  (#167 removed personal material, not authorship).

### In progress / needs the user (not delegable)

- **Launch post review** — draft ready in the companion; awaits the user's read/edits before
  it's published anywhere.
- **Action → Marketplace** — one UI step (create a v1.0.0 GitHub Release, then "Publish this
  Action"). Instant, no review.
- **GHCR package visibility** — after the first tagged release, make the auto-created package
  public (one-time, GitHub UI; can't be scripted).
- **First real Docker build** — no local docker here; the alpine image build is validated by
  the first tag push (the static binary it wraps was exercised directly).

### Done since (2026-07-04)

- **Launch FAQ (#170, completed)** — [docs/faq.md](../../docs/faq.md) pre-answers
  the four predictable critiques (preprocessor limits, C++20 modules, scope
  creep, heuristic precision) surfaced by a Qwen launch-critique analysis; linked
  from README. The post draft picked up the same three pre-emptions inline.
- **Launch post finalized** (companion `launch_drafts/launch_post_draft.md`) — four
  review rounds: drift-first lead (the user's reversal: the pain is slow per-commit
  drift, found cycles are detector evidence, not the problem; cycles demoted to one
  signal of many), two external critiques (ChatGPT) fact-checked item by item before
  applying. Ends with an agent-authorship disclosure line (user-approved).
- **Clone-signal wording fixed everywhere** — external critique caught "new-clone gate"
  claims contradicting shipped behavior (advisory, exit 0). Fixed in the post, main
  README (self-contradiction lines 46 vs 61), docs/faq.md ("structurally impossible" →
  auditable + `#if`-superset caveat), clone_tools_landscape table, duplication decision
  path, demo-repo README+title. cpparch `850911f`, demo `4062f96`.
- **r/cpp dropped, sign-up guide written (2026-07-04, this session)** — weighed the r/cpp
  compromise (one hand-written Show-and-Tell comment) by ROI and dropped the venue; it's not
  on the critical path. Revised the launch order in `paste_kit.md` (header + pre-flight) and
  wrote `launch_drafts/signup_guide.md` (per-venue register-and-post steps for isocpp /
  Meeting C++ / Show HN / Discord / LinkedIn). Companion committed + pushed. Next: user
  registers venue-by-venue, tells the agent as each is done.
- **paste_kit.md synced & pre-flight executed** — venue texts rebuilt on the final post;
  order corrected to r/cpp-first. Pre-flight all done except one item: **Marketplace
  published** (`v1.0.1` — action.yml description was over the 125-char limit, shortened;
  floating `v1` tag moved to the same commit), **GHCR package made public**, **all kit
  links browser-verified** (incl. the release-asset URL).
- **User registered on all venues; launch day locked: Tuesday 2026-07-07.** Intra-day
  schedule in the Status line above. HN mechanics confirmed to the user: URL field = the
  bare repo (no article/blog exists or is needed — the "article" role is `docs/findings` +
  the prepared first comment); Text field stays empty.
- **fd second-chance finding (HN Algolia, 2026-07-04)** — our research record said "fd's
  Show HN got 3 points"; the full record: sharkdp submitted the same repo 3× (2 pts
  2017-09-10, 3 pts 2017-09-13, **456 pts 2017-10-08**). Resubmitting the same repo weeks
  later is legitimate and can 150×. Repo-URL Show HN is the genre norm (checked: top
  1000+-point Show HNs link bare GitHub repos). → If Tuesday flops, resubmit in ~a month
  is the documented play, not a consolation.
- **Launch-day HN blocked by platform restriction (2026-07-07)** — user tried to submit
  Show HN and got HN's temporary restriction message for a massive Show HN influx, aimed
  mostly at users not yet familiar with HN culture. This is not content rejection. Decision:
  do not burn time fighting it today; continue with isocpp / Meeting C++ / Discord /
  LinkedIn, then revisit HN after account warm-up or a later second-chance window.
- **Launch-day link re-verification complete (2026-07-07)** — `curl -L` returned HTTP 200
  for the public repo, findings page, central finding, demo repo, Action repo, isocpp
  suggestion form, Meeting C++ home page, Discord invite, and HN guideline/welcome/Show HN
  pages.

### Remaining (Phase 3 — the user posts; announcements are outward-facing)

isocpp.org + Meeting C++ news → Discord / LinkedIn tail. **Show HN is deferred, not
cancelled**: return after account warm-up or a later second-chance window. Phase 4 (respond
fast, serial findings posts, CppCast, homebrew-core at stars threshold) follows.

**⚠ r/cpp DEFERRED as a launch venue (decided 2026-07-04, rules read in browser):**

Two r/cpp rules together make it a poor launch channel, and it is **not the critical path**:

1. *Project Posts rule* — personal projects go into the pinned **"Show and Tell"**
   thread as comments; top-level posts are reserved for "major, established" tools
   (Clang, GCC, Boost, fmt). The prepared §2 text of paste_kit cannot be a top-level post.
2. *AI-Generated Content rule* (2025-09-17) — bans AI-written/-polished posts **and
   comments**, so the paste_kit texts can't be posted verbatim anywhere on r/cpp.

What remains is at most one hand-written Show-and-Tell comment (user's own words): low
visibility, high author-effort, moderation risk — bad ROI. **Decision (user 2026-07-04):
drop r/cpp from the launch sequence.** It comes back only later, via modmail, once there
are adoption stories.

**Effective launch sequence (r/cpp removed, HN deferred):** isocpp.org + Meeting C++
(curated, evidence-first, in parallel) → Discord / LinkedIn tail. All active venue texts
are copy-paste-ready in the companion `paste_kit.md`; HN text stays parked for the later
second-chance attempt.

**What's left, concretely (as of 2026-07-04 EOD):**

1. **Tuesday 2026-07-07 — the user posts**: isocpp §3 + Meeting C++ §4, then Discord §6 +
   LinkedIn §7. Show HN §1 is parked because of HN's temporary restriction for the account.
2. **Phase 4 after that** (fast issue responses, serial findings posts, CppCast at
   adoption, homebrew-core at stars threshold) — plus deferred returns: r/cpp via modmail
   (needs adoption stories), static-analysis list (~late Aug 2026 + >20★), Homebrew tap
   (#166 macOS binary).
3. Optional, non-blocking: friendly-dev clean-machine installs (Phase 2 leftover),
   upstream issues for the three findings (credibility + backlinks).

### Commits (this session)

`165327f` fix(scan) fmt self-exclusion · `9449bf3` docs(process) CHANGELOG/ROADMAP sync ·
`4c30629` fix(scan) `.C` extension · `e568b1d` docs(tasks) #131 close · `05a25b0` docs(readme)
demo GIF · `9ade7ff` feat(build) GHCR image · `74d93c4` docs(license) copyright · `4735bdb`
docs(tasks) #163 Phase 1/2 progress. Companion: `55328df` launch drafts.

---

## Context — where the project actually stands

The repo is **already released**, not pre-release: tags `v0.1.0..v0.1.5`, a tag-triggered
release workflow publishing two checksummed Linux x86_64 binaries (dynamic + fully static)
with smoke jobs on clean runners and `debian:10`, a Keep-a-Changelog CHANGELOG, a live demo
repo ([blurman-ai/archcheck-demo](https://github.com/blurman-ai/archcheck-demo)), user-facing
CI/config/baseline docs, and a public-quality README. What is missing for a *public
announcement* is packaging breadth, community files, evidence write-ups, and the launch
sequence itself.

Sources for this plan: a full repo release-readiness sweep + external research on devtool
launches (opensource.guide, HN Show HN guidelines, Homebrew/Scoop/winget/AUR/GHCR/Actions
Marketplace docs, ripgrep/mold/fd launch case studies, PVS-Studio OSS-articles playbook).
Research details: ripgrep launched via a single evidence-heavy blog post with honest
anti-pitch; fd's own Show HN got 3 points and the tool still won via Reddit community posts;
mold's big HN thread was submitted by a third party. Conclusion: niche C++ venues + evidence
first, HN as a bonus.

## How maintainers decide "it's ready" (documented bar)

1. **Triable in one command** — prebuilt binary + install one-liner (HN guideline: "If your
   work isn't ready for users to try out, please don't do a Show HN").
2. **Differentiation stated** — the "why not clang-tidy/IWYU/CppDepend" table; the #1
   predictable comment in every venue.
3. **Honest evidence** — findings on real projects, hand-verified, with methodology
   (ripgrep/PVS-Studio pattern). This is exactly what #160/#161/#162 produce.
4. Repo hygiene: LICENSE, README answering what/why/quickstart/help, CONTRIBUTING,
   versioned release. (opensource.guide + GitHub community profile checklist.)

Project-specific readiness gate (from TASK_TRACKER + this session):
- [x] #160/#161/#162 manual evidence base complete — no signal ships un-audited.
      **Done 2026-07-02** (tasks in `completed/`; bugs found were fixed in #164).
- [x] #127/#131 vendored/generated sign-off (bundled-deps repos don't drown in noise).
      **Done 2026-07-02** (#131 closed → `completed/`): spdlog's bundled fmt, rocksdb
      third-party/, and terminal dep/ all excluded cleanly. Two classification bugs
      caught and fixed in the same pass (fmt self-exclusion, dep/ container gap) plus a
      case-sensitivity extension gap (`.C`). Honest residual documented, not blocking:
      #153 (fixtures, bpftrace/newsboat calibration, 4-rule path-trio gap) stays open.
      #131 Groups 2/4 (long-running corpus measurements) stay open in #123/#124 and
      #054/#066/#103 — tracked there, not gating this checklist item.
- [x] First-run sanity on 3–5 well-known OSS repos (fresh eyes, zero-config).
      **Done 2026-07-02**: fmt, spdlog, nlohmann/json, rocksdb, microsoft/terminal.
      Caught and fixed two classification bugs (fmt self-exclusion from #164 B.1;
      `dep/` container missing). All SF.9 gate cycles hand-verified as real mutual
      includes; fmt's single advisory (core.h has no include guard) is genuine.

## Phase 0 — repo hygiene (mostly done)

- [x] LICENSE (Apache-2.0) — already present.
- [x] CONTRIBUTING.md — **added 2026-07-02** (build, tests, fixture rule, one-rule-one-file,
      style pointers, commit conventions).
- [x] SECURITY.md — **added 2026-07-02** (private disclosure via GitHub security advisories).
- [x] CODE_OF_CONDUCT.md — **added 2026-07-02** (Contributor Covenant 2.1, standard).
- [x] Issue templates (bug / rule-fp / feature) + PR template — **added 2026-07-02**.
- [x] README: install section + release/license badges — **added 2026-07-02**.
- [x] README: demo screenshots or asciinema GIF (open item from #156). **Done 2026-07-02**:
      animated terminal GIF (`docs/assets/archcheck-diff-demo.gif`, 27 KB, 7 s) — real
      `--diff` output on the #156 demo repo replayed byte-for-byte (only the prompt is
      styled), embedded at the top of README. User-approved before commit. A browser
      screenshot of the live PR comment remains a nice-to-have, not a blocker.
- [x] **DECIDED 2026-07-02: rename now.** Repo renamed `blurman-ai/cpparch` →
      `blurman-ai/archcheck` before the announcement (GitHub redirects cover old links);
      README badges, docs/ci_usage.md snippets, and workflows updated in the same change.
      The local working directory stays `cpparch` (CLAUDE.md tool-path stability).
- [x] **DECIDED 2026-07-02: option (b) — journal + heavy internals go private.**
      Execution split into task **#167** (private companion repo for JOURNEY,
      milestones, dup_band, analysis/, sandbox/, symlink cleanup; backlog/ and the
      #160-#162 audit docs STAY public — they back the precision claims).
- [x] Scrub check: no secrets/sensitive material in git history (opensource.guide item).
      **Executed via #167** (2026-07-02): local paths and internal references scrubbed
      from public files and history; journal/research bulk moved to the private repo.

## Phase 1 — packaging & distribution

- [x] **Platform matrix — DECIDED 2026-07-02: NOT a launch blocker.** Launch with
      Linux x86_64; Windows x64 and macOS arm64 are planned post-launch as **#165** and
      **#166** (tasks created, ROADMAP/README note the plan). Linux arm64: on user demand.
- [x] **GitHub Action wrapper** — **shipped 2026-07-02**, public:
      [blurman-ai/archcheck-action](https://github.com/blurman-ai/archcheck-action)
      (tags `v1`, `v1.0.0`; `uses: blurman-ai/archcheck-action@v1`). Composite action:
      pinned + checksummed static binary, `--diff`, step summary, sticky PR comment via
      github-script (no third-party Marketplace dep). Remaining: publish to Actions
      Marketplace via the GitHub UI (needs a v1.0.0 GitHub Release; the user does this).
- [x] **Docker image on GHCR** — **shipped 2026-07-02** (`Dockerfile` + `docker` job in
      `release.yml`, feat commit). Alpine (not scratch — `--diff` fork/execs git),
      OCI labels, `:latest` on real releases only. NB one-time manual step on first tag:
      make the GHCR package public (created private by default). Image build not yet run
      (no local docker) — first real tag validates it.
- [ ] **Homebrew tap** — **DEFERRED 2026-07-02 until #166** (native macOS binary). Homebrew's
      audience is mostly macOS; today only linux-x86_64 ships. Formula is fully prepared and
      sha256-verified, staged in the private companion `launch_drafts/homebrew-archcheck-ready/`
      — when #166 lands, add the macOS pair and `gh repo create blurman-ai/homebrew-archcheck`.
- [ ] Nice-to-have: scoop manifest (main bucket charter = OSS CLI devtools — perfect fit),
      AUR PKGBUILD, winget manifest. pip wheel (clang-format-wheel pattern) — v0.2.
- [ ] Skip at v0.1: npm wrapper, vcpkg/conan, apt/PPA, homebrew-core.

## Phase 2 — evidence building (soft launch)

- [x] Finish #160/#161/#162 → publishable precision/FP numbers per signal. **Done 2026-07-02**
      (cycle-drift ~92%, clone 76.3%/86–91%, bool-drift 98% post-fix — sourced in the launch post).
- [x] Run archcheck on 5–10 famous C++ OSS projects; hand-verify; write up 2–3 genuinely
      interesting findings with `file:line`. **Done 2026-07-02**: `experiments/showcase/`
      010 folly (Future⇄Promise), 011 rocksdb (db.h⇄multi_scan.h), 012 terminal
      (Utils.h⇄SettingContainer.h) — all SF.9 cycles, both directions verified from git objects,
      each with an honest counter-argument. User-approved. (experiments/ is gitignored → local gallery.)
- [x] Launch blog post, ripgrep-shaped: problem (constraint decay) → pitch → **anti-pitch**
      → real findings → methodology + reproducibility. **Drafted 2026-07-02**, staged in the
      private companion `launch_drafts/launch_post_draft.md` (~1200 words). Needs user review
      before publishing anywhere.
- [ ] Optionally file upstream issues for real findings (credibility + backlinks).
- [ ] 2–3 friendly C++ devs install from scratch on clean machines; fix friction.

## Phase 3 — announcement (staggered over days, not all at once)

- [ ] **r/cpp first** (niche beats general for this tool). ⚠ Read the sidebar
      self-promotion rules in a browser first — they were not machine-verifiable.
      Technical framing: "what it found on real projects", not "I made a tool".
- [ ] isocpp.org "Suggest an Article" + Meeting C++ blogroll submission (right audience,
      trivial effort).
- [ ] **Show HN**: plain title ("Show HN: Archcheck – architecture checks for C++ in CI
      (Lakos, Core Guidelines)"), author first-comment with backstory + differentiation,
      answer everything for 2–3 h, no friend-boosting (HN bans it). A flop is normal
      (fd: 3 points) and non-fatal; the second-chance pool exists.
- [ ] awesome-cpp PR (`[archcheck](link) - description. [Apache-2.0]`, alphabetical,
      one PR), lobste.rs (author checkbox), #include<C++> Discord, X/LinkedIn.

## Phase 4 — post-launch

- [ ] Answer every issue fast for the first weeks (the documented "someone committed to
      respond" bar).
- [ ] Serial "archcheck vs <project>" findings posts (PVS-Studio/scc compounding pattern).
- [ ] Pitch CppCast / ADSP once there are adoption stories.
- [ ] homebrew-core submission at the stars threshold.

## Top-3 leverage / top-3 fatal mistakes (from the research)

Leverage: (1) prebuilt binaries + Action wrapper BEFORE any announcement; (2) evidence
post from the corpus infrastructure; (3) C++-native venues before/alongside HN.
Fatal: (1) try-it friction (build-from-source-only); (2) no differentiation vs clang-tidy;
(3) marketing tone / astroturf.

## Acceptance

- [ ] All Phase-0 items closed or explicitly decided by the user.
- [ ] Phase-1 must-haves shipped: multi-platform binaries, Action wrapper, GHCR image, tap.
- [ ] Evidence gate green: #160/#161/#162 done, showcase examples user-approved.
- [ ] Launch post drafted and user-reviewed.
- [ ] Venue sequence executed by the user (announcements are outward-facing — the user
      posts, not the agent).
