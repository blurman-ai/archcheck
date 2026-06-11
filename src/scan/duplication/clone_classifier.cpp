#include "archcheck/scan/duplication/clone_classifier.h"

namespace archcheck::scan::duplication
{

const char *cloneType(const Fragment &a, const Fragment &b)
{
  if (a.seq != b.seq)
  {
    return "STRUCTURAL";
  }
  bool idDiff = false;
  bool litDiff = false;
  for (std::size_t i = 0; i < a.seq.size(); ++i)
  {
    if (a.rawSeq[i] == b.rawSeq[i])
    {
      continue;
    }
    idDiff = idDiff || a.seq[i] == "id";
    litDiff = litDiff || a.seq[i] == "lit";
  }
  if (!idDiff && !litDiff)
  {
    return "EXACT";
  }
  if (idDiff && litDiff)
  {
    return "MIXED";
  }
  return idDiff ? "RENAMED" : "LITERAL";
}

} // namespace archcheck::scan::duplication
