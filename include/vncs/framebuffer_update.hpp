#pragma once

#include <cstdint>

namespace vncs {

struct FramebufferUpdate final
{
  std::uint16_t x;
  std::uint16_t y;
  std::uint16_t w;
  std::uint16_t h;
  const unsigned char* rgb;

  const uint32_t rawSize() const noexcept { return std::uint32_t(w) * std::uint32_t(h) * 3; }
};

} // namespace vncs
