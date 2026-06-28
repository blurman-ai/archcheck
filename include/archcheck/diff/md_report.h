#pragma once

#include <ostream>
#include <string>

#include "archcheck/diff/regression_report.h"
#include "archcheck/rules/violation.h"

namespace archcheck::diff
{

// Render a GitHub-flavoured markdown report suited to a sticky PR comment: a gate
// summary header, the advisory findings as a bullet list, and the structural diff
// folded into a <details> code block.
//
// `advisories` are the flattened advisory findings (new-clone, SATD, …). When
// `linkBase` is non-empty each finding's `file:line` is rendered as a markdown link
// `linkBase + file + "#L" + line` (e.g. a GitHub blob URL at the head commit), so the
// reader can click straight to the offending line. Empty `linkBase` ⇒ plain code spans.
void writeMdReport(const RegressionReport &r, const rules::ViolationList &advisories, const std::string &linkBase,
                   std::ostream &out);

// Convenience overload — structural report only, no advisories, no links.
void writeMdReport(const RegressionReport &r, std::ostream &out);

} // namespace archcheck::diff
