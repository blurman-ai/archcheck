# wholefile_parallel_twin — #198 whole-file guard regression

`fold_sum.cpp` and `fold_prod.cpp` are **structurally parallel** but not copies of
each other: each `fold*` function shares its per-pair kernel/adjust/scale/clamp
anchors with the matching function in the other file (so the pair is a candidate and
clears the joint floor) yet differs in one per-variant bias call, so pairs match only
at **moderate** similarity (~0.79). Embedded among them is one **byte-identical**
block, `resolveAxis`, matching at ~1.0.

The old whole-file guard counted the moderate parallel matches toward file coverage,
classed the pair as a whole-file clone, and suppressed **all** their pairs — burying
`resolveAxis`. This mirrors minsky's `node_latex.cc` / `node_matlab.cc`, where a
byte-identical 13-line hypercube-axis lookup never surfaced. The guard now counts only
near-verbatim matches toward coverage, so `resolveAxis` is reported while true
byte-copies (every fragment ~1.0) stay suppressed.

Test: `duplication_fp_guards_test.cpp`,
"P0.2: a verbatim block inside two parallel-structure files is reported, not suppressed".
