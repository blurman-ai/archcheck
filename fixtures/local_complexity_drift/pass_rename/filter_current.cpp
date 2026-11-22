// Baseline: applyRowFilter with nested branching (score above the threshold).
void applyRowFilter(const int *in, int shift, int n, int m, int *out)
{
  for (int i = 0; i < n; ++i)
  {
    for (int j = 0; j < m; ++j)
    {
      const int v = in[i * m + j];
      if (v > shift)
      {
        if (j % 2 == 0 && v < 100)
          out[i * m + j] = v - shift;
        else if (v >= 100)
          out[i * m + j] = 100;
        else
          out[i * m + j] = v;
      }
      else if (v < -shift || v == 0)
      {
        out[i * m + j] = 0;
        if (i > 0 && j > 0)
          out[i * m + j] = out[(i - 1) * m + j - 1];
      }
      while (out[i * m + j] > 50)
      {
        if (out[i * m + j] % 3 == 0)
          break;
        --out[i * m + j];
      }
    }
  }
}
