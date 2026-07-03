// Fixture A (exact-case local include) + Fixture D (external includes).
// Expected: no CASE_MISMATCH_INCLUDE, no UNRESOLVED_LOCAL_INCLUDE.
#include "core/board.hpp"

#include <SFML/Graphics.hpp>
#include <vector>

int main()
{
  std::vector<core::Board> boards(1);
  return static_cast<int>(boards.size());
}
