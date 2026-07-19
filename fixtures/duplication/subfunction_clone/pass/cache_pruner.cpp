// #190 negative fixture, other side. Structurally similar to stats_collector.cpp
// (string building, then a guarded loop) but the nested block computes eviction
// candidates, shares no distinctive tokens, and must not be reported.
#include <string>
#include <vector>

namespace cache
{

struct Entry
{
  std::string key;
  int age = 0;
  int hits = 0;
};

void pruneExpired(std::vector<Entry> &entries, std::string &log, int mode)
{
  log.clear();
  for (auto &entry : entries)
  {
    log += '[';
    log += entry.key;
    log += ']';
  }

  if (mode > 0)
  {
    int evictedCount = 0;
    int retainedCount = 0;
    for (auto &entry : entries)
    {
      if (entry.age > 3600 && entry.hits < 2)
      {
        entry.key.clear();
        ++evictedCount;
      }
      else
      {
        ++retainedCount;
      }
    }
    log += "evicted=";
    log += std::to_string(evictedCount);
    log += " retained=";
    log += std::to_string(retainedCount);
  }

  log += "\n-- prune done --\n";
}

bool hasColdEntries(const std::vector<Entry> &entries)
{
  for (const auto &entry : entries)
  {
    if (entry.hits == 0)
    {
      return true;
    }
  }
  return false;
}

} // namespace cache
