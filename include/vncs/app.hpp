#pragma once

#include <string>

#include <cstdint>

namespace vncs {

struct FramebufferUpdate;

class Client;

class App
{
public:
  struct Info final
  {
    std::string name;

    unsigned int width = 0;

    unsigned int height = 0;
  };

  virtual ~App() = default;

  virtual bool init(Client&) = 0;

  virtual void shutdown(Client&) = 0;

  virtual Info info() = 0;

  virtual void framebufferUpdateRequest(Client& client,
                                        bool incremental,
                                        std::uint16_t x,
                                        std::uint16_t y,
                                        std::uint16_t w,
                                        std::uint16_t h) = 0;

protected:
  static void sendFramebufferUpdate(Client& client,
                                    std::uint16_t x,
                                    std::uint16_t y,
                                    std::uint16_t w,
                                    std::uint16_t h,
                                    const unsigned char* rgb);

  static void sendFramebufferUpdate(Client& client, const FramebufferUpdate& update);

  static void sendFramebufferUpdates(Client& client, const FramebufferUpdate* updates, std::uint16_t updateCount);
};

} // namespace vncs
