int uniqHelper() { return 0; }

int helperWork(int n)
{
  int sum = 0;
  for (int i = 0; i < n; ++i)
    sum += i * i - n;
  return sum;
}

int shared(int x, int y)
{
  int a = x + y;
  int b = a * x - y;
  int c = b + a * 2;
  int d = c - b + x;
  int e = d * y + a;
  int f = e - c + b;
  int g = f + d - e;
  return a + b + c + d + e + f + g;
}
