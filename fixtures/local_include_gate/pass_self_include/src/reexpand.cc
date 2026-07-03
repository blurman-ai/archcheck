// Regression fixture (#168): a file that #includes itself for macro
// re-expansion (the libjxl enc_fast_lossless.cc / SIMD pattern). The resolver
// downgrades a self-edge to Unresolved, but the exact-case file plainly exists,
// so this must NOT be reported as a phantom CASE_MISMATCH against itself.
#ifndef REEXPAND_PASS
#define REEXPAND_PASS 1
#include "reexpand.cc"
#endif

int reexpand_marker() { return REEXPAND_PASS; }
