# no-inter-component-friendship

- **Category:** S (AST / semantic)
- **Authority:** high — Lakos / Bloomberg BDE
- **Source:** [BDE physical code organization](https://github.com/bloomberg/bde/wiki/physical-code-organization), Lakos *LSCSD* §3.6

## Rule

> "Friendship may be granted only within the boundaries of a single component."

## Why for archcheck

`friend` bypasses all encapsulation and is not visible in the include graph as an ordinary dependency. If module A declares `friend class B::Bar;` — A implicitly knows B's private details, any change in B may break A, but without this check archcheck won't see it. A classic hole in the architectural contract.

## Detection

AST: a `FriendDecl` inside a `CXXRecordDecl`. Determine the component that owns the class (via path/module mapping) and the component that owns the `friend` target. If they differ — flag.

Allowed: friendship between classes of the same component (a `.h`/`.cpp` pair) or within the same namespace if they are in the same file.

## Fixtures

- `pass_same_component/` — `foo.h` declares class `Foo` with `friend class FooImpl;`, `FooImpl` in `foo.cpp`.
- `fail_cross_module/` — `domain/Order.h` declares `friend class infrastructure::SqlRepo;`.
