#include <vncs/app.hpp>
#include <vncs/app_factory.hpp>
#include <vncs/server.hpp>

#include <iostream>

namespace {

class ExampleApp final : public vncs::App
{
public:
  bool init(vncs::Client&) override { return true; }

  void shutdown(vncs::Client&) override {}

  vncs::App::Info info() override
  {
    return vncs::App::Info{ "My Example App", 640, 480 };
  }
};

class ExampleAppFactory final : public vncs::AppFactory
{
public:
  std::unique_ptr<vncs::App> createApp() override
  {
    return std::unique_ptr<vncs::App>(new ExampleApp());
  }
};

} // namespace

int
main()
{
  ExampleAppFactory appFactory;

  vncs::Server server(appFactory);

  if (!server.bind()) {
    std::cerr << "Failed to bind server." << std::endl;
    return EXIT_FAILURE;
  }

  if (!server.listen()) {
    std::cerr << "Failed to start listening for connections." << std::endl;
    return EXIT_FAILURE;
  }

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);

  return EXIT_SUCCESS;
}
