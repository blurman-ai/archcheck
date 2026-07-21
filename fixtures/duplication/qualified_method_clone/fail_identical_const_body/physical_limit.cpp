#include "physical_limit.hpp"

#include <string>

namespace duckdb
{

// Trailing-qualifier body: the token before `{` is `const`, not `)`. Before #196
// the fragmenter never emitted this whole body, so this byte-identical pair
// (shared with physical_streaming_limit.cpp) was reported by no layer.
std::string PhysicalLimit::ParamsToString() const
{
  std::string result;
  result += "limit: ";
  result += std::to_string(limit_value);
  result += ", offset: ";
  result += std::to_string(offset_value);
  result += ", limit_percent: ";
  result += std::to_string(limit_percent);
  result += ", exact: ";
  result += exact_flag ? "true" : "false";
  result += ", reverse: ";
  result += reverse_flag ? "true" : "false";
  return result;
}

// A distinct, substantial fragment so the file is not a two-document island — with
// only the two identical bodies as documents, N=2 and every shared token has IDF=0,
// collapsing weighted Jaccard (a small-corpus artifact, not a clone signal).
void PhysicalLimit::InitializeState(ExecutionContext &context, GlobalState &gstate)
{
  gstate.limit_reached = false;
  gstate.current_offset = 0;
  gstate.emitted_rows = 0;
  gstate.buffer.clear();
  gstate.buffer.reserve(context.chunk_capacity);
  for (auto &partition : gstate.partitions)
  {
    partition.reset();
    partition.set_capacity(context.chunk_capacity);
  }
  gstate.initialized = true;
}

} // namespace duckdb
