#pragma once

#include <string>

#include <cstddef>

namespace vncs {

class StringView final
{
public:
  constexpr StringView(const char* data, size_t size)
    : m_data(data)
    , m_size(size)
  {}

  constexpr char operator[](size_t offset) const noexcept
  {
    if (offset < m_size)
      return m_data[offset];
    else
      return 0;
  }

  constexpr size_t size() const noexcept { return m_size; }

private:
  const char* m_data = nullptr;

  size_t m_size = 0;
};

std::string
subString(const StringView& in, size_t offset, size_t len);

} // namespace vncs
