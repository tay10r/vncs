#pragma once

#include "encoding.hpp"
#include "pixel_format.hpp"

#include <vector>

namespace vncs {

class SetPixelFormatRequest;
class SetEncodingsRequest;
class FramebufferUpdateRequest;

class MessageVisitor
{
public:
  virtual ~MessageVisitor() = default;

  virtual void visit(const SetPixelFormatRequest&) = 0;

  virtual void visit(const SetEncodingsRequest&) = 0;

  virtual void visit(const FramebufferUpdateRequest&) = 0;
};

class Message
{
public:
  virtual ~Message() = default;

  virtual void accept(MessageVisitor&) const = 0;
};

template<typename Derived>
class MessageTempl : public Message
{
public:
  virtual ~MessageTempl() = default;

  virtual void accept(MessageVisitor& visitor) const override { visitor.visit(*static_cast<const Derived*>(this)); }
};

class SetPixelFormatRequest final : public MessageTempl<SetPixelFormatRequest>
{
public:
  SetPixelFormatRequest(const PixelFormat& pixelFormat)
    : m_pixelFormat(pixelFormat)
  {}

  const PixelFormat& pixelFormat() const noexcept { return m_pixelFormat; }

private:
  PixelFormat m_pixelFormat;
};

class SetEncodingsRequest final : public MessageTempl<SetEncodingsRequest>
{
public:
  SetEncodingsRequest(std::vector<EncodingKind>&& encodings)
    : m_encodings(std::move(encodings))
  {}

  const std::vector<EncodingKind>& encodings() const noexcept { return m_encodings; }

private:
  std::vector<EncodingKind> m_encodings;
};

class FramebufferUpdateRequest final : public MessageTempl<FramebufferUpdateRequest>
{
public:
  FramebufferUpdateRequest(std::uint8_t incremental, std::uint16_t x, std::uint16_t y, std::uint16_t w, std::uint16_t h)
    : m_incremental(incremental)
    , m_x(x)
    , m_y(y)
    , m_w(w)
    , m_h(h)
  {}

  std::uint8_t incremental() const noexcept { return m_incremental; }

  std::uint16_t x() const noexcept { return m_x; }

  std::uint16_t y() const noexcept { return m_y; }

  std::uint16_t w() const noexcept { return m_w; }

  std::uint16_t h() const noexcept { return m_h; }

private:
  std::uint8_t m_incremental = 0;
  std::uint16_t m_x = 0;
  std::uint16_t m_y = 0;
  std::uint16_t m_w = 0;
  std::uint16_t m_h = 0;
};

} // namespace vncs
