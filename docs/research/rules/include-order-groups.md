# include-order-groups

- **Category:** P (preprocessor scan)
- **Authority:** medium — Google C++ Style Guide
- **Source:** [Google C++ Style Guide — Names and Order of Includes](https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes)

## Rule

> "Use standard order for readability: 1) own header (in `.cpp`), 2) C system headers, 3) C++ standard library, 4) other libraries, 5) your project's headers. Each non-empty group separated by a blank line."

## Why for archcheck

Not architecture in the strict sense, but low-hanging fruit. Indirectly checks SF.5 (`.cpp` must include its own `.h` first → if it is first, it is self-contained). Cheap, verified by a preprocessor scan without libclang. Helps with early adoption — the user sees a result on the first run without a complex config.

## Detection

Preprocessor scan: for each file, collect `#include` directives by group:
1. own header (only for `.cpp`, name matches the base name of the `.cpp`).
2. `<...>` — system or 3rd party.
3. `"..."` — project headers.

Subdivide group 2 into C system (`<sys/...>`, `<unistd.h>`, etc.), C++ stdlib (`<vector>`, `<string>`, etc.), 3rd party — by a dictionary of standard headers.

Flag:
- own header not first in the `.cpp`,
- groups not separated by a blank line,
- groups out of order (project headers before stdlib).

## Fixtures

- `pass/` — canonical order with blank lines.
- `fail_own_not_first/` — `.cpp` starts with `#include <vector>`, then `#include "foo.h"`.
- `fail_mixed_groups/` — `<vector>` next to `"foo.h"` without a separator.
