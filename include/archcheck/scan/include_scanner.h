#pragma once

#include <string_view>

#include "archcheck/scan/include_directive.h"

namespace archcheck::scan
{

ScanResult scan_includes(std::string_view source);

} // namespace archcheck::scan
