#include "message_parser.hpp"

#include "messages.hpp"
#include "string_view.hpp"

#include <cstdint>

#include <cassert>

namespace vncs {

namespace {

class MessageParserImpl final : public MessageParser
{
public:
  MessageParserImpl(const StringView& input)
    : m_input(input)
  {}

  bool errorOccurred() const override { return m_errorFlag; }

  size_t readCount() const override { return m_errorFlag ? 0 : std::min(m_offset, m_input.size()); }

  std::unique_ptr<Message> parse() override
  {
    if (atEnd())
      return nullptr;

    char messageType = peek(0);

    advance(1);

    switch (messageType) {
      case 0:
        return parseSetPixelFormatRequest();
      case 2:
        return parseSetEncodingsRequest();
      case 3:
        return parseFramebufferUpdateRequest();
      case 4:
        return parseKeyEvent();
      case 5:
        return parsePointerEvent();
      case 6:
        return parseClientCutTextEvent();
    }

    setErrorState();

    return nullptr;
  }

private:
  std::unique_ptr<Message> parseSetPixelFormatRequest()
  {
    advance(3); // <- padding

    if (remaining() < 16)
      return nullptr;

    using u16 = std::uint16_t;

    PixelFormat pixelFormat;

    pixelFormat.bitsPerPixel = peek(0);
    pixelFormat.depth = peek(1);
    pixelFormat.isBigEndian = peek(2);
    pixelFormat.isTrueColor = peek(3);
    pixelFormat.redMax = (u16(peek(4)) << 8) | u16(peek(5));
    pixelFormat.greenMax = (u16(peek(6)) << 8) | u16(peek(7));
    pixelFormat.blueMax = (u16(peek(8)) << 8) | u16(peek(9));
    pixelFormat.redShift = peek(10);
    pixelFormat.greenShift = peek(11);
    pixelFormat.blueShift = peek(12);

    advance(16);

    return std::unique_ptr<Message>(new SetPixelFormatRequest(pixelFormat));
  }

  std::unique_ptr<Message> parseSetEncodingsRequest()
  {
    advance(1); // <- padding

    if (remaining() < 2)
      return nullptr;

    const size_t encodingCount = readU16();

    if (remaining() < (encodingCount * 4))
      return nullptr;

    std::vector<EncodingKind> encodings;

    for (size_t i = 0; i < encodingCount; i++) {

      const size_t encodingID = readU32();

      const EncodingKind kind = encodingKindFromID(encodingID);

      if (kind == EncodingKind::Unknown)
        continue;

      encodings.emplace_back(kind);
    }

    return std::unique_ptr<Message>(new SetEncodingsRequest(std::move(encodings)));
  }

  std::unique_ptr<Message> parseFramebufferUpdateRequest()
  {
    if (remaining() < 9)
      return nullptr;

    const std::uint8_t incremental = readU8();
    const std::uint16_t x = readU16();
    const std::uint16_t y = readU16();
    const std::uint16_t w = readU16();
    const std::uint16_t h = readU16();

    return std::unique_ptr<Message>(new FramebufferUpdateRequest(incremental, x, y, w, h));
  }

  std::unique_ptr<Message> parseKeyEvent()
  {
    assert(false);

    return nullptr;
  }

  std::unique_ptr<Message> parsePointerEvent()
  {
    assert(false);

    return nullptr;
  }

  std::unique_ptr<Message> parseClientCutTextEvent()
  {
    assert(false);

    return nullptr;
  }

  char peek(size_t relativeOffset) const noexcept { return m_input[m_offset + relativeOffset]; }

  size_t remaining() const noexcept
  {
    if (m_offset > m_input.size())
      return 0;
    else
      return m_input.size() - m_offset;
  }

  bool atEnd() const noexcept { return m_offset >= m_input.size(); }

  void advance(size_t count) { m_offset += count; }

  void setErrorState() { m_errorFlag = true; }

  size_t readU8()
  {
    size_t c = size_t(peek(0));
    advance(1);
    return c;
  }

  size_t readU16()
  {
    size_t c = 0;
    c |= size_t(peek(0)) << 8;
    c |= size_t(peek(1));
    advance(2);
    return c;
  }

  size_t readU32()
  {
    size_t c = 0;
    c |= size_t(peek(0)) << 24;
    c |= size_t(peek(1)) << 16;
    c |= size_t(peek(2)) << 8;
    c |= size_t(peek(3));
    advance(4);
    return c;
  }

private:
  StringView m_input;

  size_t m_offset = 0;

  bool m_errorFlag = false;
};

} // namespace

std::unique_ptr<MessageParser>
MessageParser::create(const StringView& input)
{
  return std::unique_ptr<MessageParser>(new MessageParserImpl(input));
}

} // namespace vncs
