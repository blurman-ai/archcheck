# Code Quality Rules

Source: adapted from gm (`docs/ai/code_quality.md`). What to do when you see problems. You already know SOLID/DRY/YAGNI — this is about **when and how** to apply them.

## Fix Immediately (no asking)

| Issue | Action |
|-------|--------|
| Unused include/import | Delete |
| Commented-out code | Delete (git has history) |
| Unused variable/method | Delete |
| Typo in comment | Fix |
| Formatting in lines you're changing | Fix |

## Report and Suggest (ask first)

| Issue | Threshold | Why ask |
|-------|-----------|---------|
| Long function | >30 lines | May need design decision |
| Large class | >300 lines | May need module split |
| Magic number | Any | Need to agree on name |
| Copy-paste block | >5 lines repeated | Need to decide where to extract |
| SOLID violation | Any | May be intentional tradeoff |
| Missing error handling | — | Need to agree on strategy |

**Format:** "I noticed [issue] in [location]. Want me to fix it in a separate commit?"

## Thresholds

```
Function: max 30 lines (excluding braces)
Class: max 300 lines
Parameters: max 4 (else → struct)
Nesting: max 3 levels
```

Commit: `refactor(module): description` + "No functional changes"

**Never refactor:**
- In the middle of a feature
- Without passing tests
- Code you don't understand

## Anti-AI-Slop Rules

### Before Writing Code

1. **Read the file ENTIRELY first** — don't add code until you understand the existing structure
2. **Ask: can I solve this by DELETING code?** — often the answer is yes
3. **Ask: does this already exist?** — search project before creating new utilities

### Hard Limits Per Commit

| Metric | Limit |
|--------|-------|
| New lines of code | ≤50 (excluding tests/fixtures) |
| New files | ≤2 |
| New classes | ≤1 |
| New abstractions | 0 unless explicitly requested |

If task requires more — split into smaller commits with review between.

### Forbidden Patterns

```cpp
// ❌ Obvious comments
counter++;  // increment counter
if (x > 0)  // check if x is positive

// ❌ Defensive code without reason
if (ptr != nullptr) {  // "just in case"
    if (ptr->isValid()) {  // "for safety"
        if (ptr->canProcess()) {  // "to be sure"

// ❌ Premature abstraction
class IProcessor { };           // only one implementation exists
class ProcessorFactory { };     // creates one type

// ❌ Wrapper that adds nothing
Result doThing() { return actuallyDoThing(); }
```

### Class Design Rules

**NVI (Non-Virtual Interface) — public methods don't call other public methods:**

```cpp
// ❌ Bad — public calls public
class Player {
public:
    void Start() {
        Initialize();   // dangerous under inheritance
        Run();
    }
    virtual void Initialize() { }
    virtual void Run() { }
};

// ✅ Good — public calls private
class Player {
public:
    void Start() {
        DoInitialize();
        DoRun();
    }
private:
    virtual void DoInitialize() { }
    virtual void DoRun() { }
};
```

**Why:**
- Safety under inheritance — an overridden method won't break the base class's logic.
- Clear contract: public = entry points, private = implementation details.
- External code cannot call a method at the wrong moment.

**Related to:** Liskov Substitution Principle, Template Method Pattern.

### When Modifying Existing Code

1. **Match existing style exactly** — don't "improve" formatting.
2. **Don't add error handling unless there's a real bug.**
3. **Don't add logging unless debugging a specific issue.**
4. **If you're making the file longer — justify why.**

### Self-Check Before Commit

- [ ] Can I remove any code I just added?
- [ ] Are there comments that explain "what" instead of "why"?
- [ ] Did I add any "just in case" checks?
- [ ] Is every new line necessary for the task?
- [ ] Would a junior dev understand this without the comments?

## archcheck specifics

- **Each rule = one file = one class** implementing `IRule`. Adding a rule must not modify existing files (OCP).
- **Every rule must have fixtures** — `fixtures/<rule>/pass/` and `fixtures/<rule>/fail_*/`. Without fixtures, the rule does not exist.
- **Every default rule has attribution** (Core Guidelines / Lakos / Martin). If you didn't attach a reference — it's not a default, it's an option.
- **Archcheck checks itself** in CI. Any merge breaking archcheck's own SF.7/8/9/21/cycles is unacceptable.
