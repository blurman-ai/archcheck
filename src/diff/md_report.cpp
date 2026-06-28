#include "archcheck/diff/md_report.h"

#include <ostream>

#include "archcheck/diff/regression_report.h"

namespace archcheck::diff
{

namespace
{

// The gate summary line. Advisory findings lead (that is what a reviewer cares about
// on a PR); the structural gate state is appended so a red gate is never hidden.
void writeSummary(const RegressionReport &r, const rules::ViolationList &advisories, std::ostream &out)
{
  const char *icon =
      r.gates() ? ":x:" : (r.hasRegression() || !advisories.empty() ? ":large_yellow_circle:" : ":white_check_mark:");
  out << "**Gate:** " << icon << " ";
  if (!advisories.empty())
    out << advisories.size() << " advisory finding(s) · gate " << (r.gates() ? ":x: fail" : ":white_check_mark: ok");
  else if (!r.grownCycles.empty())
    out << r.grownCycles.size() << " grown cycle(s)";
  else if (!r.newGodHeaders.empty())
    out << r.newGodHeaders.size() << " new god-header(s)";
  else if (!r.addedEdges.empty())
    out << r.addedEdges.size() << " added edge(s)";
  else if (!r.newCrossAreaDependencies.empty())
    out << r.newCrossAreaDependencies.size() << " new area dep(s)";
  else
    out << "no violations";
  out << "\n";
}

} // namespace

void writeMdReport(const RegressionReport &r, const rules::ViolationList &advisories, const std::string &linkBase,
                   std::ostream &out)
{
  out << "## archcheck `--diff`\n\n";
  writeSummary(r, advisories, out);

  if (!advisories.empty())
  {
    out << "\n### Findings (advisory)\n\n";
    for (const auto &v : advisories)
    {
      out << "- ";
      if (linkBase.empty())
        out << "`" << v.file << ":" << v.line << "`";
      else
        out << "[`" << v.file << ":" << v.line << "`](" << linkBase << v.file << "#L" << v.line << ")";
      out << " — " << v.ruleId << " — " << v.message << "\n";
    }
  }

  // Structural metadata is rarely the headline on a PR — fold it away but keep it one
  // click from the comment. The code-fence preserves the column alignment GitHub
  // markdown would otherwise collapse.
  out << "\n<details><summary>Structural diff</summary>\n\n```\n";
  writeTextReport(r, out);
  out << "```\n</details>\n";
}

void writeMdReport(const RegressionReport &r, std::ostream &out) { writeMdReport(r, {}, "", out); }

} // namespace archcheck::diff
