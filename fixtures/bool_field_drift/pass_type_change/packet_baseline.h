#pragma once
#include <cstdint>

struct PacketHeader
{
  uint8_t compressed;
  uint16_t length;
  bool valid = false;
};
