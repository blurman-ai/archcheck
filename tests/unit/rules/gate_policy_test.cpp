#include <catch2/catch_test_macros.hpp>

#include "archcheck/rules/gate_policy.h"

using archcheck::rules::classifyForGate;
using archcheck::rules::countGating;
using archcheck::rules::FindingDisposition;
using archcheck::rules::GateMode;
using archcheck::rules::isGating;

TEST_CASE("gate policy: check mode gates SF.9 and CASE_MISMATCH_INCLUDE", "[rules][gate-policy]")
{
  CHECK(classifyForGate("SF.9", GateMode::Check) == FindingDisposition::Gating);
  CHECK(classifyForGate("CASE_MISMATCH_INCLUDE", GateMode::Check) == FindingDisposition::Gating);
  CHECK(classifyForGate("SF.7", GateMode::Check) == FindingDisposition::Advisory);
  CHECK(classifyForGate("Lakos.GodHeader", GateMode::Check) == FindingDisposition::Advisory);
}

TEST_CASE("gate policy: UNRESOLVED_LOCAL_INCLUDE is advisory unless the opt-in is set", "[rules][gate-policy]")
{
  CHECK(classifyForGate("UNRESOLVED_LOCAL_INCLUDE", GateMode::Check) == FindingDisposition::Advisory);
  CHECK(classifyForGate("UNRESOLVED_LOCAL_INCLUDE", GateMode::Check, /*failOnUnresolvedLocal=*/true) ==
        FindingDisposition::Gating);
  // The opt-in never touches unrelated advisories.
  CHECK(classifyForGate("Lakos.GodHeader", GateMode::Check, /*failOnUnresolvedLocal=*/true) ==
        FindingDisposition::Advisory);
}

TEST_CASE("gate policy: drift mode gates regression-grade drift rules", "[rules][gate-policy]")
{
  CHECK(isGating("DRIFT.1", GateMode::Drift));
  CHECK(isGating("DRIFT.2", GateMode::Drift));
  CHECK(isGating("DRIFT.4.CYCLE", GateMode::Drift));
  CHECK_FALSE(isGating("DRIFT.3", GateMode::Drift));
  CHECK_FALSE(isGating("DRIFT.4.NEW", GateMode::Drift));
  CHECK_FALSE(isGating("DRIFT.4.SDP", GateMode::Drift));
}

TEST_CASE("gate policy: counts gating violations", "[rules][gate-policy]")
{
  const archcheck::rules::ViolationList violations = {
      {"SF.9", "a.h", 0, "cycle"},
      {"SF.7", "b.h", 1, "using namespace"},
      {"Lakos.ChainLength", "c.h", 0, "chain"},
  };

  CHECK(countGating(violations, GateMode::Check) == 1);
}

// #171: `--diff` gates via a separate mechanism (RegressionReport::gates(),
// tests/unit/diff/regression_report_test.cpp) that has no rule-id string for
// god-headers to compare against directly. This pins the ONE documented divergence
// from this side: neither Check nor Drift mode ever gates Lakos.GodHeader, while the
// paired assertion in regression_report_test.cpp ("new god-header -> newGodHeaders
// non-empty") pins that --diff DOES gate the equivalent finding. If either side
// changes, grep for #171 to find and update its counterpart.
TEST_CASE("gate policy: check/drift vs --diff god-header divergence is exactly the documented one",
          "[rules][gate-policy]")
{
  CHECK(classifyForGate("Lakos.GodHeader", GateMode::Check) == FindingDisposition::Advisory);
  CHECK(classifyForGate("Lakos.GodHeader", GateMode::Drift) == FindingDisposition::Advisory);
  // Cycles gate on both sides of the divergence — SF.9 here, grownCycles there.
  CHECK(classifyForGate("SF.9", GateMode::Check) == FindingDisposition::Gating);
}
