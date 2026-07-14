// Three flat, sequential `if`s. Cognitive complexity counts each once with no
// nesting weight (score 3) — this is exactly what should NOT trip the check: a
// long-but-flat function is easy to read top to bottom.
void classify(int x)
{
  if (x == 1)
    tag(x);
  if (x == 2)
    tag(x);
  if (x == 3)
    tag(x);
}
