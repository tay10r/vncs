#pragma once

#include <memory>

namespace vncs {

class App;

class AppFactory
{
public:
  virtual ~AppFactory() = default;

  virtual std::unique_ptr<App> createApp() = 0;
};

} // namespace vncs
