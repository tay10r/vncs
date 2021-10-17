#pragma once

#include "encoding.hpp"
#include "pixel_format.hpp"

#include <vector>

namespace vncs {

class SetPixelFormatRequest;
class SetEncodingsRequest;

class MessageVisitor
{
public:
  virtual ~MessageVisitor() = default;

  virtual void visit(const SetPixelFormatRequest&) = 0;

  virtual void visit(const SetEncodingsRequest&) = 0;
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

  virtual void accept(MessageVisitor& visitor) const override
  {
    visitor.visit(*static_cast<const Derived*>(this));
  }
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

  const std::vector<EncodingKind>& encodings() const noexcept
  {
    return m_encodings;
  }

private:
  std::vector<EncodingKind> m_encodings;
};

} // namespace vncs
