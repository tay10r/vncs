#include <vncs/server.hpp>

#include <vncs/app_factory.hpp>

#include "client.hpp"

namespace vncs {

Server::Server(AppFactory& appFactory)
  : Server(uv_default_loop(), appFactory)
{}

Server::Server(uv_loop_t* loop, AppFactory& appFactory)
  : m_appFactory(appFactory)
{
  uv_tcp_init(loop, &m_server);

  uv_handle_set_data((uv_handle_t*)&m_server, this);
}

Server::~Server()
{
  uv_close((uv_handle_t*)&m_server, nullptr);
}

bool
Server::bind(const char* ip, int port)
{
  sockaddr_in addr;

  if (uv_ip4_addr(ip, port, &addr) != 0)
    return false;

  if (uv_tcp_bind(&m_server, (const sockaddr*)&addr, 0) != 0)
    return false;

  return true;
}

bool
Server::listen(int backlog)
{
  if (uv_listen((uv_stream_t*)&m_server, backlog, &onNewConnection) != 0)
    return false;

  return true;
}

void
Server::onNewConnection(uv_stream_t* server_stream, int status)
{
  if (status < 0)
    return;

  Server* server = (Server*)uv_handle_get_data((uv_handle_t*)server_stream);

  uv_loop_t* loop = uv_handle_get_loop((uv_handle_t*)server_stream);

  Client* client =
    new Client(loop, server->m_appFactory.createApp(), server->m_clientList);

  if (!client->initApp()) {
    client->closeAndDelete();
    return;
  }

  if (!client->accept(server_stream)) {
    client->closeAndDelete();
    return;
  }

  if (!client->startReading()) {
    client->closeAndDelete();
    return;
  }

  if (!client->initiateVNCProtocol()) {
    client->closeAndDelete();
    return;
  }
}

} // namespace vncs
