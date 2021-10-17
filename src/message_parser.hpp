#pragma once

#include <memory>

namespace vncs {

class StringView;
class Message;

class MessageParser
{
public:
  static std::unique_ptr<MessageParser> create(const StringView&);

  virtual ~MessageParser() = default;

  virtual bool errorOccurred() const = 0;

  virtual size_t readCount() const = 0;

  virtual std::unique_ptr<Message> parse() = 0;
};

} // namespace vncs
