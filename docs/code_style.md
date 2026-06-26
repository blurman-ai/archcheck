# C++20 style guide — archcheck

The base is the **LLVM coding standards**. Two deliberate departures:
**Allman braces** instead of attach, and **`name_`** (Google C++ Style trailing
underscore) for non-static fields. Any other difference from LLVM-style is
a bug in this document or in `.clang-format` — fix it with a PR.

The `.clang-format` at the repo root is authoritative for machine-checkable rules;
this document is for the meaningful decisions the formatter can't express.

## Why Allman

Allman braces are one of the two deliberate departures from LLVM-style in this
repository. The reasons: (a) a clearer visual separation of the declaration and the
body of a function/class; (b) a historically established local preference of the
maintainers. This is a stylistic choice, not a technical one — please don't
open a PR to convert to Attach.

## Why `name_` on fields

We mark non-static class fields with a trailing underscore (`name_`, not
`_name`, not `m_name`, not bare). The reasons: (a) it speeds up reading a function body —
you can tell a field from a local variable or a parameter at a glance, without
rereading the signature or scrolling to the class declaration; (b) it's Google C++
Style — a mainstream convention in C++ OSS, not an author's invention; (c) it's
technically safer than a leading underscore (`_name`), which is reserved by the
standard in a number of contexts. This is the second and last deliberate departure
from pure LLVM-style in this repository.

## Basic formatting parameters

- Encoding — UTF-8 without BOM, Unix line ending (`\n`).
- `IndentWidth: 2` (LLVM default, not 3 and not 4).
- `ColumnLimit: 120` (more tolerant than the LLVM default of 80; narrow columns aren't needed in modern IDEs).
- `BreakBeforeBraces: Allman` (our deviation — see above).
- One public type per file; helper details — in an anonymous namespace or `namespace detail`.
- We don't leave commented-out code — we have git history.

## Naming

| Category | Convention | Example |
|-----------|-----------|--------|
| Namespaces | `lower_snake_case` | `archcheck::scan` |
| Types (`class`, `struct`, `enum class`, `enum`, type `using`-aliases) | `PascalCase` | `DependencyGraph`, `IncludeKind`, `ProjectFile` |
| Methods + free functions | `lowerCamelCase` | `scanIncludes`, `addNode`, `computeScc` |
| Local variables + parameters | `lowerCamelCase` | `lineNo`, `sourceFile` |
| Fields (non-static class fields) | `lowerCamelCase` with a **trailing underscore** (Google C++ Style) | `graph_`, `lineCount_` |
| Compile-time constants (`constexpr`, `consteval`) | `kPascalCase` | `kMaxIncludeDepth` |
| Preprocessor macros and `#define` | `UPPER_SNAKE_CASE` | `ARCHCHECK_FIXTURES_DIR` |
| Template parameters | `PascalCase` (no `T` prefix) | `Allocator`, `Predicate` |

**What we do NOT do:**
- We don't prefix interfaces with `I` (`IRule`). In C++ that's a C#/MFC pattern, not our school. Just use `Rule`.
- We don't prefix fields with `_` (leading underscore). A prefix with a lowercase letter is formally legal, but `_Uppercase` and `__anything` are reserved by the standard, and many linters complain about `_name` by default. **We use a trailing underscore `name_`** — see "Why `name_` on fields" above.
- We don't distinguish `struct` and `class` by naming. Use `struct` for passive data carriers (all fields public, no invariants), `class` — for everything else.

### Compatibility with the real LLVM codebase

The canonical LLVM coding standard says `lowerCamelCase` for functions; in the
real codebase (LLDB, historical parts of Clang) `snake_case` also occurs.
For new code the LLVM community is in fact drifting toward `lowerCamelCase` —
we fix this as the target and don't look back at the historical
pieces of LLVM.

## File content order

1. `#pragma once` (header) / includes (cpp).
2. `#include`/`import` in the order: standard library → third-party → internal (`"archcheck/..."`).
3. `using` declarations, no more than one or two, in an anonymous namespace in the .cpp.
4. Type and function declarations.

`#include` blocks are separated by blank lines; clang-format with
`IncludeBlocks: Regroup` does this automatically.

## Preprocessor and constants

Macros — only for conditional compilation, platform attributes, or
threading paths through the build system (see `ARCHCHECK_FIXTURES_DIR`).
Numeric constants — `constexpr` / `consteval`.

```cpp
#pragma once

#include <cstddef>

namespace archcheck::config
{

inline constexpr std::size_t kMaxIncludeDepth = 10;
inline constexpr std::size_t kGodHeaderFanIn = 30;

}  // namespace archcheck::config
```

## Enumerations

`enum class` with an explicit underlying type, when relevant. Plain `enum` —
only for C-APIs.

```cpp
enum class Severity : std::uint8_t
{
  Info,
  Warning,
  Error,
};
```

## Structs

`struct` — for passive data carriers. All fields public, initialized
in place, no invariants. **Struct fields — without a trailing underscore**: they're
part of the public data interface, not the internal state of a class (this coincides
with Google C++ Style).

```cpp
struct SourceLocation
{
  std::string file;
  std::uint32_t line = 0;
  std::uint32_t column = 0;
};
```

## Classes and interfaces

Pure-virtual bases — a plain `class` (no `I` prefix).
A virtual destructor is mandatory. Contracts — through attributes
(`[[nodiscard]]`, `noexcept`).

```cpp
class Rule
{
public:
  virtual ~Rule() = default;

  [[nodiscard]] virtual std::string_view id() const noexcept = 0;
  [[nodiscard]] virtual std::vector<Violation> check(const ComponentGraph& graph) const = 0;
};
```

For concrete classes:

- Constructors — `explicit`, unless copy/move.
- The initializer list line by line, in field declaration order.
- The rule of five — explicitly `= default` or `= delete`.
- Resources — RAII through the standard library.
- Non-static fields — with a trailing underscore (`name_`).

```cpp
class FileScanner
{
public:
  explicit FileScanner(const std::filesystem::path& root) : root_(root) {}

  [[nodiscard]] std::size_t scannedCount() const noexcept { return scannedCount_; }
  void scan();

private:
  std::filesystem::path root_;
  std::size_t scannedCount_ = 0;
};
```

## Functions and methods

- `[[nodiscard]]` on any return that's worth checking.
- `noexcept` by default, if the function doesn't throw.
- Return instead of output parameters: `std::optional`, `std::pair`, `std::tuple`, `std::expected` (in v0.2+).
- Complex return types — `auto` + trailing return type.

## Control constructs

`if constexpr` for template selection, range-based `for` for collections,
`std::ranges` for composition.

### Braceless conditionals

A single-line body — braces aren't required:

```cpp
if (value < 0)
  return;

for (auto& item : items)
  item.update();
```

**Symmetric bracing**: if one branch of an `if/else` is multi-line — braces are
needed **everywhere**:

```cpp
// OK — both branches single-line
if (ready)
  process();
else
  wait();

// OK — else is multi-line → braces everywhere
if (ready)
{
  process();
}
else
{
  logError();
  wait();
}

// NOT ALLOWED — asymmetry
if (ready)
  process();
else
{
  logError();
  wait();
}
```

### Compact switch

Single-line actions — align by columns:

```cpp
switch (severity)
{
  case Severity::Info:    formatInfo(out, v);    break;
  case Severity::Warning: formatWarning(out, v); break;
  case Severity::Error:   formatError(out, v);   break;
}
```

## C++20

- **Concepts** for requirements on template parameters.
- **Ranges / views** for composing transformations.
- **`std::span` / `std::string_view`** — buffers and strings without copying.
- **`std::optional` / `std::variant`** instead of flags and incomplete structs.
- **`consteval` / `constexpr` / `constinit`** for compile-time computations.
- **Attributes:** `[[nodiscard]]`, `[[maybe_unused]]`, `[[noreturn]]`, `[[likely]]`, `[[unlikely]]`.

## Comments

- Public APIs — Doxygen (`///` or `/** */`).
- Comments explain **why**, not **what**. Above the block, not at the end of the line.
- Complex algorithms — references to the specification (Lakos, Core Guidelines, Martin).

## Tooling

- **`clang-format-18`** in CI (ubuntu-24.04 native + matches `clang-18` in the build matrix). Parameters — see the root `.clang-format`. The version is pinned: different major versions format slightly differently (especially `AlignAfterOpenBracket`, `SortUsingDeclarations`), CI runs `--dry-run --Werror`. If CI is red on format, **fix it locally**:

  ```bash
  sudo apt-get install -y clang-format-18           # installs from Astra/Debian repos
  find src include tests -name '*.h' -o -name '*.cpp' | xargs clang-format-18 -i --style=file
  ```

  A reformat commit — separate, no semantic changes, the SHA in `.git-blame-ignore-revs`.
- `clang-tidy` — the profiles `modernize-*`, `cppcoreguidelines-*`, `readability-*`, `bugprone-*`, `performance-*`.
- `lizard --CCN 15 --length 30 --arguments 5 --warnings_only` locally before every push.

## archcheck specifics

archcheck is itself an architecture-checking tool, so **dogfooding
is mandatory**. The archcheck code must pass archcheck in CI:

- No cycles in the include graph (SF.9).
- No `using namespace` in `.h` (SF.7).
- Every `.h` self-contained, with `#pragma once` (SF.8, SF.11).
- No anonymous namespaces in headers (SF.21).
- Each rule — a separate file, a separate class, registration through a static table (OCP). Adding a new rule must not edit existing rule files.
</content>
