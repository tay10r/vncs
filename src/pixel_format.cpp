#include "pixel_format.hpp"

#include "binary_writer.hpp"

namespace vncs {

BinaryWriter&
operator<<(BinaryWriter& writer, const PixelFormat& pixelFormat)
{
  writer << pixelFormat.bitsPerPixel;
  writer << pixelFormat.depth;
  writer << pixelFormat.isBigEndian;
  writer << pixelFormat.isTrueColor;
  writer << pixelFormat.redMax;
  writer << pixelFormat.greenMax;
  writer << pixelFormat.blueMax;
  writer << pixelFormat.redShift;
  writer << pixelFormat.greenShift;
  writer << pixelFormat.blueShift;
  writer << uint8_t(0); // padding
  writer << uint8_t(0); // padding
  writer << uint8_t(0); // padding

  return writer;
}

} // namespace vncs
