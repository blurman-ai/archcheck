// Guard clauses / early returns are cheap in cognitive complexity — each is a
// flat `if` (no nesting), so the function stays well below any alarm even with
// several exits.
int parse(int x)
{
  if (x < 0)
    return -1;
  if (x == 0)
    return 0;
  if (x > 100)
    return 100;
  return x;
}
