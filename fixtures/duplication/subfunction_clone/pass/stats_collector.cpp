// #190 negative fixture: same SHAPE as the fail case (a guarded loop nested inside a
// larger function) but different logic — different accumulator, different guard,
// different output. Descending into function bodies must not turn "both files happen
// to contain an if-guarded for-loop" into a reported clone.
#include <string>
#include <vector>

namespace stats
{

struct Sample
{
  std::string tag;
  int value = 0;
  int bucket = 0;
};

void collectPercentiles(std::vector<Sample> &samples, std::string &out, int mode)
{
  out.clear();
  for (auto &sample : samples)
  {
    out += sample.tag;
    out += '=';
    out += std::to_string(sample.value);
    out += ';';
  }

  if (mode > 0)
  {
    int peakValue = 0;
    int troughValue = 0;
    for (auto &sample : samples)
    {
      if (sample.value > peakValue)
      {
        peakValue = sample.value;
      }
      if (sample.value < troughValue)
      {
        troughValue = sample.value;
      }
    }
    out += "peak=";
    out += std::to_string(peakValue);
    out += " trough=";
    out += std::to_string(troughValue);
  }

  out += "\n-- percentiles --\n";
}

int distinctBuckets(const std::vector<Sample> &samples)
{
  int seen = 0;
  for (const auto &sample : samples)
  {
    if (sample.bucket >= 0)
    {
      ++seen;
    }
  }
  return seen;
}

} // namespace stats
