#pragma once

#include <string>

namespace vncs {

class Security
{
public:
  virtual ~Security() = default;

  virtual std::string Decrypt(const std::string&, size_t) const = 0;

  virtual std::string Encrypt(const std::string&, size_t) const = 0;
};

} // namespace vncs
