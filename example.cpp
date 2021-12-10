#include <vncs/app.hpp>
#include <vncs/app_factory.hpp>
#include <vncs/server.hpp>

#include <cassert>

#include <iostream>

namespace {

#define WIDTH 640

#define HEIGHT 480

class ExampleApp final : public vncs::App
{
public:
  bool init(vncs::Client&) override { return true; }

  void shutdown(vncs::Client&) override {}

  vncs::App::Info info() override { return vncs::App::Info{ "My Example App", WIDTH, HEIGHT }; }

  void framebufferUpdateRequest(vncs::Client& client,
                                bool incremental,
                                std::uint16_t x0,
                                std::uint16_t y0,
                                std::uint16_t w,
                                std::uint16_t h) override
  {
    w = 640;
    h = 480;

    assert((x0 + w) <= WIDTH);
    assert((y0 + h) <= HEIGHT);

    std::cout << x0 << ' ' << y0 << ' ' << w << ' ' << h << std::endl;

    std::unique_ptr<unsigned char[]> rgb(new unsigned char[w * h * 3]);

    const std::uint16_t x1 = x0 + w;
    const std::uint16_t y1 = y0 + h;

    static int updateCount = 0;

    for (std::uint16_t y = y0; y < y1; y++) {

      for (std::uint16_t x = x0; x < x1; x++) {

        const float u = (x + 0.5f) / WIDTH;
        const float v = (y + 0.5f) / HEIGHT;

        const std::uint32_t i = (std::uint32_t(y) * WIDTH) + std::uint32_t(x);

        rgb.get()[(i * 3) + 0] = u * 255;
        rgb.get()[(i * 3) + 1] = v * 255;
        rgb.get()[(i * 3) + 2] = updateCount;
      }
    }

    updateCount = (updateCount + 1) % 256;

    sendFramebufferUpdate(client, x0, y0, w, h, rgb.get());
  }

  void keyEvent(bool pressed, std::uint32_t key) override
  {
    std::cout << "key event: " << (pressed ? "'press'" : "'release'") << " " << char32_t(key) << std::endl;
  }

  void pointerEvent(std::uint8_t buttonMask, std::uint16_t x, std::uint16_t y) override
  {
    (void)buttonMask;

    std::cout << "pointer event: " << x << ' ' << y << std::endl;
  }
};

class ExampleAppFactory final : public vncs::AppFactory
{
public:
  std::unique_ptr<vncs::App> createApp() override { return std::unique_ptr<vncs::App>(new ExampleApp()); }
};

} // namespace

int
main()
{
  uv_loop_t* loop = uv_default_loop();

  ExampleAppFactory appFactory;

  vncs::Server server(loop, appFactory);

  if (!server.bind()) {
    std::cerr << "Failed to bind server." << std::endl;
    return EXIT_FAILURE;
  }

  if (!server.listen()) {
    std::cerr << "Failed to start listening for connections." << std::endl;
    return EXIT_FAILURE;
  }

  uv_run(loop, UV_RUN_DEFAULT);

  return EXIT_SUCCESS;
}
