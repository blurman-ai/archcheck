// Fixture B: the header on disk is include/core/board.hpp, but the directive
// spells it "core/Board.hpp". Resolves only when case is ignored — a build break
// on a case-sensitive filesystem. Expected: CASE_MISMATCH_INCLUDE = 1 (gating).
#include "core/Board.hpp"

int main()
{
  return core::Board{}.size;
}
