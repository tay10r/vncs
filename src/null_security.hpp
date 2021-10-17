#pragma once

#include "security.hpp"

namespace vncs {

class NullSecurity final : public Security
{
public:
  std::string Decrypt(const std::string&, size_t) const override;

  std::string Encrypt(const std::string&, size_t) const override;
};

} // namespace vncs
