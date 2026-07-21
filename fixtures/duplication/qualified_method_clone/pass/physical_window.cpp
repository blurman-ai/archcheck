#include "physical_window.hpp"

#include <string>

namespace duckdb
{

// Also a const method — same qualifier shape as physical_hash_join.cpp, entirely
// different body. Must not be reported as a clone.
std::string PhysicalWindow::ParamsToString() const
{
  std::string text;
  text.reserve(64);
  text.append("window over ");
  text.append(std::to_string(partition_count));
  text.append(" partitions, frame=");
  text.append(frame_mode == FrameMode::RANGE ? "range" : "rows");
  text.append(", peers=");
  text.append(exclude_peers ? "excluded" : "included");
  return text;
}

} // namespace duckdb
