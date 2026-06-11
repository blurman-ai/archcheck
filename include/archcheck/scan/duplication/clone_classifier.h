#pragma once

#include "archcheck/scan/duplication/fragmenter.h"

namespace archcheck::scan::duplication
{

// Classify clone type based on how fragments diverge:
//   EXACT      identical token-for-token and spelling
//   RENAMED    only local identifiers differ; normalized stream intact
//   LITERAL    only literals differ; normalized stream intact
//   MIXED      both identifiers and literals differ; normalized stream intact
//   STRUCTURAL normalized streams themselves diverge (Type-3)
const char *cloneType(const Fragment &a, const Fragment &b);

} // namespace archcheck::scan::duplication
