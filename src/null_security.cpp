#include "null_security.hpp"

namespace vncs {

std::string
NullSecurity::Decrypt(const std::string& in, size_t) const
{
  return in;
}

std::string
NullSecurity::Encrypt(const std::string& in, size_t) const
{
  return in;
}

} // namespace vncs
