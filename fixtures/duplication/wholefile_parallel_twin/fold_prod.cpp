// Twin (prod variant). Each fold* shares its per-pair kernel/adjust/scale/clamp anchors
// with the matching function in the other file (so the pair is a candidate and clears
// the joint floor) but differs in one per-variant bias call — the two files are
// structurally PARALLEL, not byte-copies. resolveAxis is byte-identical and must
// surface: the whole-file guard used to bury it with the parallel noise (#198).
#include <cstddef>
#include <vector>
namespace demo
{
double foldMag(const std::vector<double> &xs, double seed)
{
  double acc = seed;
  for (std::size_t i = 0; i < xs.size(); ++i)
  {
    acc = magKernel(acc, xs[i]);
    acc = magAdjust(acc, i);
    acc = magScale(acc, seed);
    acc = magClamp(acc, i);
    acc = prodBias(acc, seed);
  }
  return acc;
}
double foldSpan(const std::vector<double> &xs, double seed)
{
  double acc = seed;
  for (std::size_t i = 0; i < xs.size(); ++i)
  {
    acc = spanKernel(acc, xs[i]);
    acc = spanAdjust(acc, i);
    acc = spanScale(acc, seed);
    acc = spanClamp(acc, i);
    acc = prodBias(acc, seed);
  }
  return acc;
}
double foldTrace(const std::vector<double> &xs, double seed)
{
  double acc = seed;
  for (std::size_t i = 0; i < xs.size(); ++i)
  {
    acc = traceKernel(acc, xs[i]);
    acc = traceAdjust(acc, i);
    acc = traceScale(acc, seed);
    acc = traceClamp(acc, i);
    acc = prodBias(acc, seed);
  }
  return acc;
}
long resolveAxis(const std::vector<long> &dims, long axis)
{
  long index = -1;
  for (std::size_t i = 0; i < dims.size(); ++i)
  {
    if (dims[i] == axis)
    {
      index = static_cast<long>(i);
      break;
    }
  }
  if (index < 0 && !dims.empty())
  {
    index = 0;
  }
  return index;
}
} // namespace demo
