#include "physical_streaming_limit.hpp"

#include <string>

namespace duckdb
{

// Byte-identical to PhysicalLimit::ParamsToString() — a real copy-paste between two
// operator implementations. The trailing `const` is what hid it from the fragmenter.
std::string PhysicalStreamingLimit::ParamsToString() const
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

// A distinct, substantial fragment so the file is not a two-document island (see the
// note in physical_limit.cpp) — unrelated logic, must not clone against it.
void PhysicalStreamingLimit::Finalize(GlobalState &gstate, ClientContext &client)
{
  gstate.merged = true;
  gstate.flush_pending = false;
  client.profiler.EndPhase();
  for (auto &chunk : gstate.pending_chunks)
  {
    chunk.Verify();
    client.registry.Track(chunk.size());
  }
  gstate.pending_chunks.clear();
}

} // namespace duckdb
