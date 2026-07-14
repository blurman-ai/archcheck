// Long boolean chains add to cognitive complexity: each run of a like operator
// costs, and every switch between `&&` and `||` costs again. But without nesting
// the total stays modest, so a realistic mixed-logic condition passes the default
// ceiling — nesting, not boolean width, is what makes the score explode.
bool eligible(int a, int b, int c, int d, int e, int f, int g)
{
  if (a && b && c || d || e && f || g && a || b && c || d && e || f && g)
    return true;
  return false;
}
