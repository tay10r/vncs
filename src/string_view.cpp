#include "string_view.hpp"

namespace vncs {

std::string
subString(const StringView& in, size_t offset, size_t len)
{
  std::string out(len, ' ');

  for (size_t i = 0; i < len; i++)
    out[i] = in[offset + i];

  return out;
}

} // namespace vncs
