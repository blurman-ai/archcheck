int uniqHelper() { return 0; }

int helperWork(int n)
{
  int sum = 0;
  for (int i = 0; i < n; ++i)
    sum += i * i - n;
  return sum;
}
