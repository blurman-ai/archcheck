#pragma once

#include <string>
#include <vector>

#include "archcheck/rules/violation.h"
#include "archcheck/scan/duplication/token_normalizer.h"

namespace archcheck::scan
{

// Flag-argument heuristic (ARG.1 + ARG.2, #093). Flags interface drift toward
// boolean flags — a cheap "behaviour is leaking into the interface" early-warning,
// NOT a strict semantic rule. Token-level, advisory.
//
//   ARG.1.flag_argument_signature (over discovered function signatures):
//     - >= 2 boolean parameters in one signature, or
//     - exactly 1 boolean parameter whose name reads like a flag
//       (enable/disable/use/force/skip/with/without/no/is/allow, on a word
//       boundary), so `bool node` does not trip but `bool enableX` does.
//     `bool*` / `bool&` are NOT counted — a pointer/reference is an out-parameter,
//     a different pattern. Declarations and definitions dedup by name+arity.
//
//   ARG.2.boolean_literal_call (over call sites):
//     - a call passing >= 2 boolean literals positionally ("f(true, false)"),
//       where each literal's meaning is opaque at the call. Control headers
//       (if/for/while/switch/catch) are keywords, never call candidates; a function
//       definition's own parameter parens are skipped (default args are ARG.1).
[[nodiscard]] rules::ViolationList detectFlagArguments(const std::vector<duplication::Token> &tokens,
                                                       const std::string &file);

} // namespace archcheck::scan
