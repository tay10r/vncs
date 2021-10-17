#pragma once

namespace vncs {

enum class EncodingKind
{
  Raw,
  CopyRect,
  RRE,
  Hextile,
  TRLE,
  ZRLE,
  Unknown
};

constexpr EncodingKind
encodingKindFromID(size_t id)
{
  switch (id) {
    case 0:
      return EncodingKind::Raw;
    case 1:
      return EncodingKind::CopyRect;
    case 2:
      return EncodingKind::RRE;
    case 5:
      return EncodingKind::Hextile;
    case 15:
      return EncodingKind::TRLE;
    case 16:
      return EncodingKind::ZRLE;
  }

  return EncodingKind::Unknown;
}

} // namespace vncs
