# Boolean accretion: when "+N bools" is fine, and when it's a smell

**Thesis of this note.** The `n_bool_field_added` metric (#135: net increase of depth-0 bool fields
in a *pre-existing* structure per commit) measures **accumulation of boolean state —
a neutral phenomenon, not a verdict of "bad code"**. From a single added bool, you cannot
conclude "there it is, the drift". Added bools fall into two visually indistinguishable classes:

- **Legitimate, orthogonal** — independent binary properties; any combination of them is
  valid. Honest domain modeling, not a defect.
- **Boolean-blindness smell** — several bools encode **mutually exclusive or
  correlated** states, and an invalid combination is *representable in the type*, even though
  unreachable in meaning. It cries out for an `enum` / sum type. This is exactly constraint decay
  ([constraint_decay.md](constraint_decay.md), Dente et al., EURECOM 2026).

## Reference example — both classes in one commit

**GregorGullwi/FlashCpp** (hobby C++ compiler), commit `b968556`,
`src/AstNodeTypes.h`, class `LambdaExpressionNode`. A single commit added 3 bool fields
(was 1 → became 4 — real drift into a live structure):

```cpp
bool is_mutable_;     // ┐ independent, orthogonal — a lambda can be
bool is_noexcept_;    // ┘ any combination of mutable × noexcept       → VALID
bool is_constexpr_;   // ┐ MUTUALLY EXCLUSIVE: in C++ a lambda cannot be
bool is_consteval_;   // ┘ both constexpr AND consteval at once        → SMELL
```

- `is_mutable_` / `is_noexcept_` — two independent properties, all 4 combinations are valid.
  This is **not** a defect, it is a correct model of lambda syntax.
- `is_constexpr_` + `is_consteval_` encode a three-valued state (neither / constexpr /
  consteval) with two bools. The combination `(true, true)` is representable, but unreachable in
  C++: a lambda cannot be both `constexpr` and `consteval` at once. The invariant "not both at
  once" is nowhere expressed in the type — it rests on discipline. The canonical fix is `enum class
  LambdaConstevalKind { None, Constexpr, Consteval }`.

That is, **in one and the same commit, valid bools and a smell sit side by side** — the accretion
metric does not distinguish them, and should not.

## Consequences

1. **For the product (#090, deferred).** Surface bool drift as **advisory**, not error:
   the bare fact of `+N bools` does not prove a defect. Otherwise the rule will produce false
   positives on honest domain modeling.
2. **For interpretation (#119).** The judge "is bool accumulation a proxy for real trouble?" is a
   **correlation** (does accretion grow together with copy-paste / complexity), not a raw count.
   The metric is a neutral quantum; the conclusion comes from correlation analysis.
3. **For the showcase / findings.** Don't slap a "bad drift" label on every commit with
   `+N bools`. The "boolean-blindness" class (mutually exclusive states encoded with N bools) is a
   separate, stronger signal than plain accretion; only domain knowledge (like `constexpr ⊥
   consteval`) or correlation can tell it apart.

## Related

- [constraint_decay.md](constraint_decay.md) — the root cause (boolean blindness as decay).
- [boolean_state_perstruct_drift.md](boolean_state_perstruct_drift.md) — the history axis
  (a structure accumulates bools over ≥4 commits; **auto-generated**, do not edit by hand).
- [boolean_state_enum_analysis.md](boolean_state_enum_analysis.md) — analysis of "boolean
  combinations → enum".
- #135 — per-commit metric `n_bool_field_added` (sidecar + join under #119).
