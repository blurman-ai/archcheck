// Fixture C: a quoted local include that resolves nowhere in the project (no
// file, no case-insensitive match). May be generated or depend on a custom
// include root, so it is advisory by default; --fail-on-unresolved-local gates
// it. Expected: UNRESOLVED_LOCAL_INCLUDE = 1.
#include "generated/config.hpp"

int main()
{
  return 0;
}
