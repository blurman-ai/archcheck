# interface-pure-abstract

- **Category:** S (AST / semantic)
- **Authority:** high — C++ Core Guidelines
- **Source:** [CCG C.121 — If a base class is used as an interface, make it a pure abstract class](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c121-if-a-base-class-is-used-as-an-interface-make-it-a-pure-abstract-class)

## Rule

> "An interface should have no data and no implementation. Mixing concerns gets in the way."

## Why for archcheck

Reinforces Martin's *Abstractness* (A) — turns an "interface" layer into a genuinely interface one. If a base class is named `IFoo` / `FooInterface` (or is simply used only as an inheritance point), it must be pure abstract. Otherwise the "interface" drags an implementation along — a module that was supposed to depend on the abstraction clings to the concrete.

## Detection

AST pass: a class that:
- is inherited by at least one other class, **and**
- has at least one virtual function.

Check: all non-static functions are pure virtual; no data members; there is a virtual destructor.

Optionally under a "strict" flag — flag only classes with the prefix `I` or the suffix `Interface`.

## Fixtures

- `pass/` — `class IFoo { virtual ~IFoo() = default; virtual void Do() = 0; };`.
- `fail_data/` — an interface with `int _state;`.
- `fail_impl/` — an interface with a non-empty method body.
- `fail_no_virtual_dtor/` — all methods pure, but no `virtual ~`.
