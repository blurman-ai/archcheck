---
name: Rule false positive / false negative
about: A rule fired on legitimate code, or missed a real violation
labels: rule-quality
---

**Version**

Output of `archcheck --version`:

**Rule**

Which finding, verbatim (e.g. `a.h: [SF.9] cycle: ...` or a `DRIFT.*` line):

**Command**

```bash
archcheck ...
```

**Why this is wrong**

Explain why the flagged code is legitimate (false positive), or paste code that should
have been flagged but wasn't (false negative).

**Code**

A minimal sample, or a link to the public repo + commit. For `--diff` findings, the
revspec you used.
