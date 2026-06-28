int uniqTwo() { return 0; }

// Reformatted in this commit: a comment added and the braces reflowed. The
// normalized token sequence is unchanged, so the parent-guard recognizes this
// as a pre-existing clone and stays silent (advisory must not fire).
int shared(int x, int y) {
  int a = x + y;
  int b = a * x - y;
  int c = b + a * 2;
  int d = c - b + x;
  int e = d * y + a;
  int f = e - c + b;
  int g = f + d - e;
  return a + b + c + d + e + f + g;
}
