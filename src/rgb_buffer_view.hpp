#pragma once

#include <cstdint>

namespace vncs {

struct RGBBufferView final
{
  const unsigned char* data = nullptr;
  std::uint16_t w = 0;
  std::uint16_t h = 0;
};

} // namespace vncs
