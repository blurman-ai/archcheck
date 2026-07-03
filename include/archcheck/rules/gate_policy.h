#pragma once

#include <cstddef>
#include <string_view>

#include "archcheck/rules/violation.h"

namespace archcheck::rules
{

enum class GateMode
{
  Check,
  Drift
};

enum class FindingDisposition
{
  Advisory,
  Gating
};

// `failOnUnresolvedLocal` (the --fail-on-unresolved-local opt-in) promotes
// UNRESOLVED_LOCAL_INCLUDE from advisory to gating; CASE_MISMATCH_INCLUDE always
// gates in Check mode. Both default off so existing call sites keep their meaning.
[[nodiscard]] FindingDisposition classifyForGate(std::string_view ruleId, GateMode mode,
                                                 bool failOnUnresolvedLocal = false) noexcept;
[[nodiscard]] bool isGating(std::string_view ruleId, GateMode mode, bool failOnUnresolvedLocal = false) noexcept;
[[nodiscard]] std::size_t countGating(const ViolationList &violations, GateMode mode,
                                      bool failOnUnresolvedLocal = false);

} // namespace archcheck::rules
