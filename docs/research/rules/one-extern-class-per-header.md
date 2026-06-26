# one-extern-class-per-header

- **Category:** S (AST / semantic)
- **Authority:** medium — HIC++ 4.0
- **Source:** [HIC++ 4.0, rule 3.3.1](https://abougouffa.github.io/awesome-coding-standards/hi-cpp-4.0.pdf)

## Rule

> "Define each externally visible class in a separate header."

## Why for archcheck

A direct consequence of CRP/SDP: if one `.h` declares five public classes, any client that wants to use one of them is forced to transitively pull in all five. This inflates the module's real fan-in and lowers its modularity. One public class = one header — a canonical BDE convention that also supports SF.5 (a paired `.cpp`).

## Detection

AST: for each `.h`, count `CXXRecordDecl`s (classes and structs) with external linkage that are not nested. If > 1 — flag.

Allowed:
- nested classes (via `Foo::Inner`),
- classes in `namespace detail` or `namespace internal` (by convention — private implementation details),
- forward declarations (declaration only, not definition),
- POD structs without methods (helper data carriers — behind a flag).

## Fixtures

- `pass_one_class/` — `foo.h` declares `class Foo`, everything else in `detail`.
- `pass_nested/` — `foo.h` declares `class Foo` with nested `class Foo::Builder`.
- `fail_two_externals/` — `foo.h` declares `class Foo` and `class Bar`, both external.
