#pragma once

#include <string>

namespace vncs {

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
};

} // namespace vncs
