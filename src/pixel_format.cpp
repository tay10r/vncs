#include "pixel_format.hpp"

namespace vncs {

std::string
PixelFormat::toBinaryString() const
{
  std::string str(size_t(16), '\0');

  str[0] = bitsPerPixel;
  str[1] = depth;
  str[2] = isBigEndian;
  str[3] = isTrueColor;
  str[4] = redMax >> 8;
  str[5] = redMax;
  str[6] = greenMax >> 8;
  str[7] = greenMax;
  str[8] = blueMax >> 8;
  str[9] = blueMax;
  str[10] = redShift;
  str[11] = greenShift;
  str[12] = blueShift;
  str[13] = 0; // padding
  str[14] = 0; // padding
  str[15] = 0; // padding

  return str;
}

} // namespace vncs
