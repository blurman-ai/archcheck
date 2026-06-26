# file-suffix

- **Category:** P (include-only / filesystem scan)
- **Authority:** high — C++ Core Guidelines
- **Source:** [CCG NL.27 — Use a `.cpp` suffix for code files and `.h` for interface files](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#nl27-use-a-cpp-suffix-for-code-files-and-h-for-interface-files)

## Rule

> "Long-standing convention. `.cpp` for source files, `.h` for headers."

## Why for archcheck

Trivially checked, catches real clutter (`.cc` alongside `.cpp`, `.hpp` alongside `.h`), gives the user a quick win on the first run. Allows configuration — a project may choose `.cc/.hh` or `.cpp/.hpp`, the key thing is uniformity.

## Detection

Filesystem-scan of the project root: collect source and header suffixes, count frequencies. If a file with a minority suffix is found — flag. Allowed pair sets (one per project):
- `.cpp` + `.h`
- `.cpp` + `.hpp`
- `.cc` + `.h`
- `.cxx` + `.hxx`

## Fixtures

- `pass_cpp_h/` — only `.cpp` + `.h`.
- `pass_cpp_hpp/` — only `.cpp` + `.hpp`.
- `fail_mixed/` — `foo.cpp` alongside `bar.cc`.
- `fail_outlier_header/` — all `.h`, one `.hxx` file.
