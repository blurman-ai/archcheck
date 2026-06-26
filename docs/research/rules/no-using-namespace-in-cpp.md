# no-using-namespace-in-cpp

- **Category:** S (AST / semantic) — the simple case is also P (text scan)
- **Authority:** medium — Google C++ Style Guide
- **Source:** [Google C++ Style Guide — Namespaces / Unnamed Namespaces and Static Variables](https://google.github.io/styleguide/cppguide.html#Namespaces)

## Rule

> "You may not use a `using`-directive to make all names from a namespace available. Do not use namespace aliases at namespace scope in header files except in explicitly marked internal-only namespaces."

## Why for archcheck

SF.7 forbids `using namespace` only in headers. This rule is a stricter mode, an optional flag for high-discipline projects: `using namespace` is forbidden everywhere, including `.cpp`. The benefit: names don't "leak" into the TU and the reader sees a clear prefix indicating where a symbol comes from. Optional, not a default.

## Detection

AST: a `UsingDirectiveDecl` at namespace scope in any project file. Allowed:
- inside a function body (local scope),
- in `namespace { ... }` (anonymous namespace in a `.cpp`) — it doesn't actually "leak" outward.

## Fixtures

- `pass_function_local/` — `void f() { using namespace std::chrono_literals; ... }`.
- `pass_anonymous/` — `namespace { using namespace std::ranges; }` in a `.cpp`.
- `fail_file_scope/` — `using namespace std;` at the start of a `.cpp`.
- `fail_in_namespace/` — `namespace archcheck { using namespace std; }` in a `.cpp`.
