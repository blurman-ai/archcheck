// #190 fixture: the shared code is a BLOCK nested inside a function, not a whole
// function. Everything around it differs, so the enclosing bodies are not clones —
// only the nested block is. Before #190 the fragmenter emitted each function body
// whole and never descended, so this class was invisible.
#include <string>
#include <vector>

namespace reporting
{

struct Row
{
  std::string label;
  int weight = 0;
  int offset = 0;
};

void writeSummaryReport(std::vector<Row> &rows, std::string &out, int mode)
{
  out.clear();
  out.reserve(rows.size() * 32);
  for (auto &row : rows)
  {
    out += row.label;
    out += ':';
    out += std::to_string(row.weight);
    out += '\n';
  }

  // ---- the copied block ----
  if (mode > 0)
  {
    int normalizedTotal = 0;
    int discardedRows = 0;
    for (auto &row : rows)
    {
      if (row.weight <= 0)
      {
        ++discardedRows;
        continue;
      }
      normalizedTotal += row.weight * 100 / (row.offset + 1);
    }
    out += "total=";
    out += std::to_string(normalizedTotal);
    out += " discarded=";
    out += std::to_string(discardedRows);
  }
  // ---- end copied block ----

  out += "\n-- end of summary --\n";
}

int countHeavyRows(const std::vector<Row> &rows, int limit)
{
  int seen = 0;
  for (const auto &row : rows)
  {
    if (row.weight > limit)
    {
      ++seen;
    }
  }
  return seen;
}

} // namespace reporting
