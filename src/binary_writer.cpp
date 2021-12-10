#include "binary_writer.hpp"

#include <ostream>

namespace vncs {

void
BinaryWriter::write(const unsigned char* bytes, size_t size)
{
  m_output.write((const char*)bytes, size);
}

BinaryWriter&
operator<<(BinaryWriter& writer, ServerMessageType type)
{
  switch (type) {
    case ServerMessageType::FramebufferUpdate:
      return writer << std::uint8_t(0);
  }

  return writer;
}

BinaryWriter&
operator<<(BinaryWriter& writer, EncodingKind encodingKind)
{
  switch (encodingKind) {
    case EncodingKind::Raw:
      return writer << std::uint8_t(0);
    case EncodingKind::CopyRect:
      return writer << std::uint8_t(1);
    case EncodingKind::RRE:
      return writer << std::uint8_t(2);
    case EncodingKind::Hextile:
      return writer << std::uint8_t(5);
    case EncodingKind::TRLE:
      return writer << std::uint8_t(15);
    case EncodingKind::ZRLE:
      return writer << std::uint8_t(16);
  }

  return writer;
}

BinaryWriter&
operator<<(BinaryWriter& writer, const RGBBufferView& rgb)
{
  const uint32_t size = std::uint32_t(rgb.w) * std::uint32_t(rgb.h);

  for (std::uint32_t i = 0; i < size; i++) {
    writer << rgb.data[(i * 3) + 0];
    writer << rgb.data[(i * 3) + 1];
    writer << rgb.data[(i * 3) + 2];
    writer << std::uint8_t(0);
  }

  return writer;
}

} // namespace vncs
