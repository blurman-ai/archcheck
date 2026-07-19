// #190 fixture, other side. The nested block below is a verbatim copy of the one in
// report_writer.cpp; the surrounding function is unrelated (CSV escaping, not text
// summary), so the two enclosing bodies must NOT match on their own.
#include <string>
#include <vector>

namespace exporting
{

struct Row
{
  std::string label;
  int weight = 0;
  int offset = 0;
};

void exportDelimited(std::vector<Row> &rows, std::string &out, int mode, char sep)
{
  out.clear();
  for (auto &row : rows)
  {
    bool needsQuote = row.label.find(sep) != std::string::npos;
    if (needsQuote)
    {
      out += '"';
      out += row.label;
      out += '"';
    }
    else
    {
      out += row.label;
    }
    out += sep;
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

  out += '\n';
}

bool anyLabelEmpty(const std::vector<Row> &rows)
{
  for (const auto &row : rows)
  {
    if (row.label.empty())
    {
      return true;
    }
  }
  return false;
}

} // namespace exporting
