#include "archcheck/rules/gate_policy.h"

#include <algorithm>

namespace archcheck::rules
{

FindingDisposition classifyForGate(std::string_view ruleId, GateMode mode, bool failOnUnresolvedLocal) noexcept
{
  if (mode == GateMode::Check)
  {
    if (ruleId == "SF.9" || ruleId == "CASE_MISMATCH_INCLUDE")
      return FindingDisposition::Gating;
    if (ruleId == "UNRESOLVED_LOCAL_INCLUDE")
      return failOnUnresolvedLocal ? FindingDisposition::Gating : FindingDisposition::Advisory;
    return FindingDisposition::Advisory;
  }

  return (ruleId == "DRIFT.1" || ruleId == "DRIFT.2" || ruleId == "DRIFT.4.CYCLE") ? FindingDisposition::Gating
                                                                                   : FindingDisposition::Advisory;
}

bool isGating(std::string_view ruleId, GateMode mode, bool failOnUnresolvedLocal) noexcept
{
  return classifyForGate(ruleId, mode, failOnUnresolvedLocal) == FindingDisposition::Gating;
}

std::size_t countGating(const ViolationList &violations, GateMode mode, bool failOnUnresolvedLocal)
{
  return static_cast<std::size_t>(std::count_if(violations.begin(), violations.end(), [&](const auto &v)
                                                { return isGating(v.ruleId, mode, failOnUnresolvedLocal); }));
}

} // namespace archcheck::rules
