#include "physical_hash_join.hpp"

#include <string>

namespace duckdb
{

// A const method, but its body is unrelated to the aggregate one — broadening the
// body heuristic must not turn "both are const methods" into a false clone.
std::string PhysicalHashJoin::ParamsToString() const
{
  std::string out = "join[";
  out += JoinTypeName(join_type);
  out += "] on ";
  for (const auto &cond : conditions)
  {
    out += cond.left_column;
    out += " = ";
    out += cond.right_column;
    out += "; ";
  }
  out += "build=" + std::to_string(build_side);
  return out;
}

} // namespace duckdb
