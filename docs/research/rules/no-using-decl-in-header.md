# no-using-decl-in-header

- **Category:** S (AST / semantic)
- **Authority:** medium — HIC++ 4.0
- **Source:** [HIC++ 4.0, rule 7.3.6](https://abougouffa.github.io/awesome-coding-standards/hi-cpp-4.0.pdf)

## Rule

> "Do not write `using` declarations at namespace scope in a header."

## Why for archcheck

Complements SF.7 (`using namespace`). SF.7 catches the "directive" (`using namespace std;`); this rule catches the "declaration" (`using std::vector;`) — it too injects a name into the header's namespace, and too leaks into every TU that `#include`s that header. Each case seems harmless on its own, but in aggregate they create name pollution.

## Detection

AST: a `UsingDecl` at namespace scope in a file that is part of the include graph (i.e. a header or a file someone `#include`s). Allowed:
- inside a class body (this is an alias for inheritance),
- inside a function body,
- in `namespace detail { ... }` if the project config permits (behind a flag).

## Fixtures

- `pass_class_using/` — `class Derived : Base { using Base::method; };`.
- `pass_function_local/` — `void f() { using std::vector; ... }`.
- `fail_namespace_scope/` — in `foo.h`: `using std::vector;` at namespace level.
