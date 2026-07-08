int funcA(int x, int y)
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

int funcB(int x, int y)
{
  int a = x - y;
  int b = a + x * y;
  int c = b - a * 3;
  int d = c + b - x;
  int e = d - y * a;
  int f = e + c - b;
  int g = f - d + e;
  return a - b + c - d + e - f + g;
}

int funcC(int x, int y)
{
  int a = x * y;
  int b = a - x + y;
  int c = b * a + 4;
  int d = c + b * x;
  int e = d * y - a;
  int f = e + c * b;
  int g = f * d + e;
  return a * b - c + d * e - f + g;
}
