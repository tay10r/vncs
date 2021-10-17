#pragma once

#include <vncs/client_list.hpp>

#include <memory>

#include <uv.h>

namespace vncs {

class AppFactory;

class Server final
{
public:
  Server(AppFactory& app_factory);

  Server(uv_loop_t* loop, AppFactory& app_factory);

  ~Server();

  bool bind(const char* ip = "0.0.0.0", int port = 5900);

  bool listen(int backlog = 128);

  uv_loop_t* loop() { return uv_handle_get_loop((uv_handle_t*)&m_server); }

private:
  static void onNewConnection(uv_stream_t*, int status);

private:
  uv_tcp_t m_server;

  AppFactory& m_appFactory;

  std::shared_ptr<ClientList> m_clientList = std::make_shared<ClientList>();
};

} // namespace vncs
