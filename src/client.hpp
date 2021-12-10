#pragma once

#include <vncs/app.hpp>
#include <vncs/client_list.hpp>

#include "security.hpp"

#include <uv.h>

#include <memory>

namespace vncs {

class FramebufferUpdateRequest;
class StringView;

class ClientState
{
public:
  struct Ok final
  {};

  struct Error final
  {
    const char* reason;
  };

  struct ReadResult final
  {
    size_t readSize;

    bool errorOccurred;

    static ReadResult ok(size_t readSize) { return ReadResult{ readSize, false }; }

    static ReadResult error() { return ReadResult{ 0, true }; }
  };

  virtual ~ClientState() = default;

  virtual ReadResult read(const StringView&, Client&) = 0;

protected:
  void setSecurity(Client& client, std::unique_ptr<Security>&& security);

  void sendSecurityResult(Client&, Ok);

  void sendSecurityResult(Client&, const Error&);

  void sendSupportedSecurityTypes(Client& client);

  bool sendRawData(Client&, std::string&& data);

  void setState(Client& client, std::unique_ptr<ClientState>&& state);
};

class Client final
{
public:
  Client(uv_loop_t* loop, std::unique_ptr<App>&& app, const std::shared_ptr<ClientList>& clientList);

  bool initApp();

  void shutdownApp();

  App::Info appInfo();

  bool accept(uv_stream_t* server_stream);

  bool startReading();

  bool initiateVNCProtocol();

  void closeAndDelete();

  void setReadLimit(size_t limit) { m_readLimit = limit; }

  void makeExclusive();

  bool sendRawData(std::string&& data);

  void framebufferUpdateRequest(const FramebufferUpdateRequest& request);

  void keyEvent(bool down, std::uint32_t key);

private:
  friend ClientState;

  void setState(std::unique_ptr<ClientState>&& state);

  static void allocBuffer(uv_handle_t*, size_t, uv_buf_t*);

  static void onRead(uv_stream_t*, ssize_t, const uv_buf_t*);

  static std::unique_ptr<Security> createDefaultSecurity();

private:
  uv_tcp_t m_client;

  std::string m_buffer;

  size_t m_readSize = 0;

  size_t m_readLimit = 1024 * 1024;

  std::unique_ptr<ClientState> m_clientState;

  std::unique_ptr<Security> m_security = createDefaultSecurity();

  std::unique_ptr<App> m_app;

  std::shared_ptr<ClientList> m_clientList;
};

} // namespace vncs
