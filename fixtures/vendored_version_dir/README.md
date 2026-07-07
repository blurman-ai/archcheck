# vendored_version_dir fixtures (#179)

On-disk trees for the version+qualifier vendored-dir classifier
(`net-snmp-<ver>-final-patched/`). Each subdir is a distinct scan root, consumed
by `tests/integration/graph/vendor_exclude_test.cpp`.

- `pass/` — build root. `libfoo-2d-renderer/foo.cpp` is an authored lookalike:
  it has a digit tail but its stem (`libfoo`) is NOT a curated vendored token, so
  it must stay in the graph.
- `pass_self_project/net-snmp/` — build root is the `net-snmp` dir itself. The
  scan root's own name (net-snmp → netsnmp) exempts a same-token versioned
  subtree from the vendored drop, so `net-snmp-5.8-final-patched/snmpwalk.c` stays.
- `fail_netsnmp/` — build root. `src/net-snmp-5.8-final-patched/snmpwalk.cpp`
  must be classified vendored and dropped; `src/app.cpp` (authored) stays as a
  contrast so the drop is not confused with an empty graph.
