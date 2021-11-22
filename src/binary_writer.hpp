#pragma once

#include <iosfwd>

#include <cstddef>
#include <cstdint>

#include "encoding.hpp"
#include "rgb_buffer_view.hpp"
#include "server_message_type.hpp"

namespace vncs {

class BinaryWriter final
{
public:
  BinaryWriter(std::ostream& output)
    : m_output(output)
  {}

  void write(const unsigned char* bytes, size_t byteCount);

private:
  std::ostream& m_output;
};

inline BinaryWriter&
operator<<(BinaryWriter& writer, uint8_t value)
{
  writer.write(&value, 1);
  return writer;
}

inline BinaryWriter&
operator<<(BinaryWriter& writer, std::uint16_t value)
{
  const uint8_t buf[2]{ std::uint8_t(value >> 8), std::uint8_t(value) };
  writer.write(buf, 2);
  return writer;
}

inline BinaryWriter&
operator<<(BinaryWriter& writer, std::uint32_t value)
{
  // clang-format off
  const uint8_t buf[4]{
    std::uint8_t(value >> 24),
    std::uint8_t(value >> 16),
    std::uint8_t(value >> 8),
    std::uint8_t(value)
  };
  // clang-format on
  writer.write(buf, 4);
  return writer;
}

BinaryWriter&
operator<<(BinaryWriter& writer, ServerMessageType type);

BinaryWriter&
operator<<(BinaryWriter& writer, EncodingKind encodingKind);

BinaryWriter&
operator<<(BinaryWriter& writer, const RGBBufferView& rgb);

} // namespace vncs
