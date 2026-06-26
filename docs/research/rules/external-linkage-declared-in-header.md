# external-linkage-declared-in-header

- **Category:** S (AST / semantic)
- **Authority:** high — Bloomberg BDE / Lakos
- **Source:** [BDE physical code organization](https://github.com/bloomberg/bde/wiki/physical-code-organization)

## Rule

> "All external-linkage constructs defined in a `.cpp` file must be declared in the corresponding `.h` file."

## Why for archcheck

Catches the "shadow API": a function/variable defined in a `.cpp` without `static`/`anonymous namespace`, not declared in the `.h`, but with external linkage — it can be linked from a foreign TU via an `extern` declaration. This bypasses the header-as-the-only-entry-point, a direct hole in the architectural contract: the module promises an interface through the `.h`, but actually lets you in through the back door.

## Detection

For each `.cpp`:
1. Find all `FunctionDecl`s and `VarDecl`s with external linkage (not `static`, not in an `anonymous namespace`, not `inline`).
2. Read the paired `.h` (per the SF.5 rule) and collect the declared names.
3. If something in the `.cpp` is not declared in the `.h` — flag.

Ignore: implicit template instantiation, `extern "C"` API within its own layer.

## Fixtures

- `pass/` — all `.cpp` functions have a declaration in the `.h`.
- `fail_undeclared_function/` — `.cpp` contains `void Helper() { ... }` (external linkage), the `.h` does not know about it.
- `pass_static/` — the same function marked `static` — allowed.
- `pass_anonymous_namespace/` — a function in `namespace { ... }` — allowed.
