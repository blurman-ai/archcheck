#include "archcheck/scan/local_complexity_absolute.h"

#include <string>

#include "archcheck/scan/local_complexity_metrics.h"

namespace archcheck::scan
{

rules::ViolationList detectAbsoluteComplexity(const SourceSnapshot &snapshot, std::size_t threshold)
{
  rules::ViolationList result;
  for (const auto &file : snapshot.files())
  {
    if (!file.authored)
    {
      continue;
    }
    for (const auto &fn : computeFileComplexity(file.content))
    {
      if (static_cast<std::size_t>(fn.score) < threshold)
      {
        continue;
      }
      result.push_back({"FUNC.COGNITIVE_COMPLEXITY", file.path, fn.startLine,
                        "function '" + fn.qualifiedName + "' cognitive complexity " + std::to_string(fn.score) +
                            " exceeds threshold " + std::to_string(threshold)});
    }
  }
  return result;
}

} // namespace archcheck::scan
