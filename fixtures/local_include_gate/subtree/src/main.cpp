// Subtree-scan fixture: the header lives in ../include (outside a scan rooted
// at src/), exact case. With the enclosing-repo universe the include is silent;
// scanning src/ alone (no universe) it is Unresolved.
#include "core/lib.hpp"

int main()
{
  return core::answer();
}
