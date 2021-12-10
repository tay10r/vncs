#pragma once

#include <cstdint>

namespace vncs {

class BinaryWriter;

struct PixelFormat final
{
  std::uint8_t bitsPerPixel;
  std::uint8_t depth;
  std::uint8_t isBigEndian;
  std::uint8_t isTrueColor;

  std::uint16_t redMax;
  std::uint16_t greenMax;
  std::uint16_t blueMax;

  std::uint8_t redShift;
  std::uint8_t greenShift;
  std::uint8_t blueShift;
};

BinaryWriter&
operator<<(BinaryWriter& writer, const PixelFormat&);

} // namespace vncs
