#pragma once
#include <cstdint>

struct PacketHeader
{
  bool compressed;
  uint16_t length;
  bool valid = false;
};
