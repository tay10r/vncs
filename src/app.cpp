#include <vncs/app.hpp>

#include <vncs/framebuffer_update.hpp>

#include "binary_writer.hpp"
#include "client.hpp"

#include <sstream>

namespace vncs {

void
App::sendFramebufferUpdate(Client& client,
                           std::uint16_t x,
                           std::uint16_t y,
                           std::uint16_t w,
                           std::uint16_t h,
                           const unsigned char* rgb)
{
  return sendFramebufferUpdate(client, FramebufferUpdate{ x, y, w, h, rgb });
}

void
App::sendFramebufferUpdate(Client& client, const FramebufferUpdate& update)
{
  return sendFramebufferUpdates(client, &update, size_t(1));
}

void
App::sendFramebufferUpdates(Client& client, const FramebufferUpdate* updates, std::uint16_t updateCount)
{
  std::ostringstream stream;

  BinaryWriter writer(stream);

  writer << ServerMessageType::FramebufferUpdate;

  writer << uint8_t(0); // Padding

  writer << updateCount;

  for (std::uint16_t i = 0; i < updateCount; i++) {
    writer << updates[i].x;
    writer << updates[i].y;
    writer << updates[i].w;
    writer << updates[i].h;
    writer << EncodingKind::Raw;
  }

  for (std::uint16_t i = 0; i < updateCount; i++)
    writer << RGBBufferView{ updates[i].rgb, updates[i].w, updates[i].h };

  client.sendRawData(stream.str());
}

} // namespace vncs
