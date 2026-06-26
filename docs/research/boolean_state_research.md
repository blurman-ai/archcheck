# Boolean-State Drift: Research Sources & Authority Anchors

**Date:** 2026-06-07  
**Status:** draft (gathered sources, needs deep research for completeness)  
**Task:** #089

## Hypothesis

A class that accumulates boolean state fields (`started`, `running`, `paused`, `failed`, ...) encodes an implicit state machine. N flags → 2^N theoretical states; the growth of the gap between legal and representable states may serve as a drift signal.

## Authority Anchors

### C++ Core Guidelines

- **ES.21: "Do not use a variable for two unrelated purposes"** — boolean flags accumulating in a single class often violate this principle, mixing different aspects of state.
- **C.130: "Avoid virtual functions for type-dependent operations"** — related to the State Pattern as an alternative to a multitude of bool flags.
- **F.26: "Use T* or owner<T> to designate a single object"** — naming and semantics of flags.

### Lakos (Large-Scale C++ Software Design)

- **Levelization violations & CCD growth** — growth in boolean fields often correlates with violations of physical design and increasing CCD.
- **God-headers & God-objects** — classes that accumulate many bool fields often evolve into "god objects" that violate SRP.

### Gang of Four & Martin

- **State Pattern** (GoF) — an explicit replacement for a multitude of boolean flags.
- **Make Illegal States Unrepresentable** (Martin, "Your Code as a Crime Scene") — a key principle for detection: if a system allows representing illegal states (failed && completed), that is a design defect.

### Recent research & blogs

- **"Boolean Soup"** — a common term in the community for classes with many bool fields without structure.
- **"State Explosion"** — the problem of combinations of boolean flags: N flags = 2^N states, but the handling often does not cover them all.
- **"Boolean Blindness"** — the inability to type boolean arguments (a separate pattern, but related to state flags).
- **"Flag Arguments"** (Uncle Bob, Clean Code) — considered an anti-pattern, especially in such combinations.

## Existing tools & Smell Detectors

### Static Analyzers

- **Cppcheck** — can detect some boolean conditions, but not state explosion as such.
- **clang-tidy** — has checks for "too many arguments", but not for boolean-state accumulation.
- **SonarQube** — has rules on boolean conditions & complexity, but not specific to state flags.

### Academic work

- Looking for: smell detectors, AST-based state-machine inference, boolean-complexity metrics.
- Areas: program verification, model checking (theory), but little applied to real C++ codebases.

## Examples (from experience)

### Textbook bad case

```cpp
class NetworkConnection {
  bool connected;      // ← related to state
  bool authenticated;  // ← related to state
  bool encrypted;      // ← related to state
  bool reconnecting;   // ← related to state
  
  // 16 theoretical states, ~3-4 legal
  // Impossible combinations:
  // - connected=false && authenticated=true
  // - encrypted=true && !connected
  // - reconnecting=true && connected=true
};
```

### Good case (State Pattern)

```cpp
class NetworkConnection {
  std::unique_ptr<ConnectionState> state;  // ← explicit state machine
};
class ConnectionState {
  virtual ~ConnectionState() = default;
  virtual void onConnect(NetworkConnection& ctx);
  virtual void onAuthenticate(NetworkConnection& ctx);
};
// Legal states explicitly enumerated, impossible ones excluded
```

## Next phases (stubs)

### Corpus Study
- [ ] Scan the OSS corpus for classes with 3+ bool fields.
- [ ] Rank by number of flags.
- [ ] Extraction: libclang AST vs grep/regex (document FP/FN).

### State-Likeness Heuristics
- [ ] Naming: state flags (`started/running/paused/failed/ready/connected`) vs config (`enable_*/use_*`).
- [ ] Conditional expressions: how often are several bool flags combined?

### Real-world Examples (≥20)
- [ ] Collect examples from the corpus with analysis of "impossible states".

### Drift Simulation
- [ ] For each example: N → 2^N, estimate legal states, gap.

### Tooling Gap
- [ ] Existing: SonarQube, Cppcheck, clang-tidy, CppDepend, Designite, PMD.
- [ ] Gap: no specific tools for state-machine detection in C++.

### Feasibility
- [ ] Candidate rules: `boolean_state_growth`, `implicit_state_machine`, `state_flag_accumulation`.
- [ ] CI-suitability: semantic backend (#042) or grep heuristic?
- [ ] Verdict: YES / NO / MAYBE.

---

**Status:** draft, needs deep research on web sources for completeness (articles, talks, academic papers).
