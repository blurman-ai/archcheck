// Nesting is what the metric punishes: each control structure adds its current
// depth as extra weight, so an `if` inside a loop inside an `if` inside a loop
// costs far more than the same keywords laid out flat. This compact function
// scores well past the default ceiling (score 45).
void scan(int n, int m)
{
  for (int a = 0; a < n; ++a) // +1
  {
    if (a % 2 == 0) // +2
    {
      for (int b = 0; b < m; ++b) // +3
      {
        if (b > a) // +4
        {
          for (int c = 0; c < b; ++c) // +5
          {
            if (c > b) // +6
            {
              for (int d = 0; d < c; ++d) // +7
              {
                if (d > c) // +8
                {
                  if (d == n) // +9
                    emit(a, b, c, d);
                }
              }
            }
          }
        }
      }
    }
  }
}
