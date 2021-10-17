#include "client.hpp"

#include "message_parser.hpp"
#include "messages.hpp"
#include "null_security.hpp"
#include "pixel_format.hpp"
#include "string_view.hpp"

#include <sstream>
#include <string>

#include <cstdint>
#include <cstring>

#include <iostream>

namespace vncs {

namespace {

void
onClose(uv_handle_t* handle)
{
  Client* client = (Client*)uv_handle_get_data(handle);

  delete client;
}

std::string
u32ToBinaryString(std::uint32_t n)
{
  using u32 = std::uint32_t;

  using uchar = unsigned char;

  const uchar buf[4]{ uchar(n >> 24), uchar(n >> 16), uchar(n >> 8), uchar(n) };

  std::string out;

  out.resize(4);

  for (int i = 0; i < 4; i++)
    out[i] = buf[i];

  return out;
}

std::string
u16ToBinaryString(std::uint16_t n)
{
  using u16 = std::uint32_t;

  using uchar = unsigned char;

  std::string out(size_t(2), ' ');
  out[0] = uchar(n >> 8);
  out[1] = uchar(n);
  return out;
}

class WriteOperation final
{
public:
  static WriteOperation* create(std::string&& data)
  {
    return new WriteOperation(std::move(data));
  }

  bool execute(uv_stream_t* stream)
  {
    return uv_write(
             &m_handle, stream, &m_buffer, 1, &WriteOperation::onWrite) == 0;
  }

private:
  WriteOperation(std::string&& data)
    : m_data(std::move(data))
  {
    m_buffer.base = &m_data[0];

    m_buffer.len = m_data.size();

    uv_handle_set_data((uv_handle_t*)&m_handle, this);
  }

  static void onWrite(uv_write_t* handle, int)
  {
    WriteOperation* op =
      (WriteOperation*)uv_handle_get_data((uv_handle_t*)handle);

    delete op;
  }

private:
  std::string m_data;

  uv_buf_t m_buffer;

  uv_write_t m_handle;
};

class EstablishedClientState;

class MessageHandler final : public MessageVisitor
{
public:
  void visit(const SetPixelFormatRequest&);

private:
};

class EstablishedClientState final : public ClientState
{
public:
  ReadResult read(const StringView& inputBuffer, Client& client) override
  {
    std::unique_ptr<MessageParser> messageParser =
      MessageParser::create(inputBuffer);

    std::unique_ptr<Message> message = messageParser->parse();

    if (!message) {
      if (messageParser->errorOccurred())
        return ReadResult::error();

      return ReadResult::ok(0);
    }

    std::cout << "got message: " << messageParser->readCount() << std::endl;

    (void)client;

    return ReadResult::ok(messageParser->readCount());
  }

private:
};

class ClientInitReader final : public ClientState
{
public:
  ReadResult read(const StringView& inputBuffer, Client& client) override
  {
    if (inputBuffer.size() < 1)
      return ReadResult::ok(0);

    bool sharedFlag = inputBuffer[0];

    if (!sharedFlag)
      client.makeExclusive();

    sendServerInit(client);

    setState(client, std::unique_ptr<ClientState>(new EstablishedClientState));

    return ReadResult::ok(1);
  }

private:
  void sendServerInit(Client& client)
  {
    const PixelFormat pixelFormat{
      32, 24, false, true, 255, 255, 255, 16, 8, 0
    };

    const App::Info info = client.appInfo();

    std::ostringstream stream;

    stream << u16ToBinaryString(info.width);
    stream << u16ToBinaryString(info.height);

    stream << pixelFormat.toBinaryString();

    stream << u32ToBinaryString(info.name.size());
    stream << info.name;

    sendRawData(client, stream.str());
  }
};

class SecurityTypeReader final : public ClientState
{
public:
  ReadResult read(const StringView& inputBuffer, Client& client) override
  {
    if (inputBuffer.size() < 1)
      return ReadResult{ 0, false };

    switch (inputBuffer[0]) {
      case 0x01: // None
        sendSecurityResult(client, Ok{});
        setSecurity(client, std::unique_ptr<Security>(new NullSecurity()));
        setState(client, std::unique_ptr<ClientState>(new ClientInitReader()));
        return ReadResult{ 1, false };
      case 0x02: // VNC Authentication
        break;
    }

    sendSecurityResult(client,
                       Error{ "Encryption/Authentication not supported." });

    return ReadResult{ 0, true };
  }
};

class ClientVersionReader final : public ClientState
{
public:
  static ReadResult error() { return ReadResult{ 0, true }; }

  ReadResult read(const StringView& inputBuffer, Client& client) override
  {
    if (inputBuffer.size() < 12)
      return ReadResult{ 0, false };

    if ((inputBuffer[0] != 'R') || (inputBuffer[1] != 'F') ||
        (inputBuffer[2] != 'B'))
      return error();

    std::string majorVerStr = subString(inputBuffer, 4, 3);

    std::string minorVerStr = subString(inputBuffer, 8, 3);

    const int majorVer = std::atoi(majorVerStr.c_str());
    const int minorVer = std::atoi(minorVerStr.c_str());

    if (majorVer != 3)
      return error();

    if (minorVer != 8)
      return error();

    if (inputBuffer[11] != '\n')
      return error();

    setState(client, std::unique_ptr<ClientState>(new SecurityTypeReader()));

    sendSupportedSecurityTypes(client);

    return ReadResult{ 12, false };
  }
};

} // namespace

void
ClientState::setState(Client& client, std::unique_ptr<ClientState>&& state)
{
  client.m_clientState = std::move(state);
}

void
ClientState::setSecurity(Client& client, std::unique_ptr<Security>&& security)
{
  client.m_security = std::move(security);
}

void
ClientState::sendSupportedSecurityTypes(Client& client)
{
  // First byte is the number of security types, the following bytes are the
  // codes for each security type.
  //
  // In this case, there is only two security types:
  //  - 0x01: which is the 'None' security type.
  //  - 0x02: which is VNC authentication

  // std::string securityTypes("\x02\x01\x02", 3);
  std::string securityTypes("\x01\x01", 2);

  WriteOperation* writeOp = WriteOperation::create(std::move(securityTypes));

  writeOp->execute((uv_stream_t*)&client.m_client);
}

void
ClientState::sendSecurityResult(Client& client, Ok)
{
  WriteOperation* op = WriteOperation::create(u32ToBinaryString(0));

  op->execute((uv_stream_t*)&client.m_client);
}

void
ClientState::sendSecurityResult(Client& client, const Error& error)
{
  size_t reason_length = std::strlen(error.reason);

  std::ostringstream resultStream;

  resultStream << u32ToBinaryString(1);

  resultStream << u32ToBinaryString(reason_length);

  resultStream << error.reason;

  std::string resultStr = resultStream.str();

  WriteOperation* op = WriteOperation::create(std::move(resultStr));

  op->execute((uv_stream_t*)&client.m_client);
}

bool
ClientState::sendRawData(Client& client, std::string&& data)
{
  return client.sendRawData(std::move(data));
}

Client::Client(uv_loop_t* loop,
               std::unique_ptr<App>&& app,
               const std::shared_ptr<ClientList>& clientList)
  : m_app(std::move(app))
  , m_clientList(clientList)
{
  uv_tcp_init(loop, &m_client);

  uv_handle_set_data((uv_handle_t*)&m_client, this);
}

void
Client::closeAndDelete()
{
  m_app->shutdown(*this);

  m_clientList->remove(this);

  uv_close((uv_handle_t*)&m_client, &onClose);
}

bool
Client::initApp()
{
  return m_app->init(*this);
}

void
Client::shutdownApp()
{
  return m_app->shutdown(*this);
}

App::Info
Client::appInfo()
{
  return m_app->info();
}

bool
Client::accept(uv_stream_t* server_stream)
{
  if (uv_accept(server_stream, (uv_stream_t*)&m_client) != 0)
    return false;

  return true;
}

bool
Client::startReading()
{
  return uv_read_start(
           (uv_stream_t*)&m_client, &Client::allocBuffer, &Client::onRead) == 0;
}

bool
Client::initiateVNCProtocol()
{
  m_clientState.reset(new ClientVersionReader());

  WriteOperation* writeOp = WriteOperation::create("RFB 003.008\n");

  return writeOp->execute((uv_stream_t*)&m_client);
}

void
Client::makeExclusive()
{
  m_clientList->makeExclusive(this);
}

void
Client::allocBuffer(uv_handle_t* handle, size_t suggestedSize, uv_buf_t* buf)
{
  Client* client = (Client*)uv_handle_get_data(handle);

  const size_t oldSize = std::min(client->m_readSize, client->m_buffer.size());

  client->m_buffer.resize(oldSize + suggestedSize);

  buf->base = &client->m_buffer[oldSize];

  buf->len = suggestedSize;
}

void
Client::onRead(uv_stream_t* stream, ssize_t readSize, const uv_buf_t*)
{
  Client* client = (Client*)uv_handle_get_data((uv_handle_t*)stream);

  if (readSize < 0) {
    client->closeAndDelete();
    return;
  }

  if (!client->m_clientState)
    return;

  client->m_readSize += readSize;

  const std::string inputString =
    client->m_security->Decrypt(client->m_buffer, client->m_readSize);

  const StringView inputBuffer(inputString.data(), inputString.size());

  const ClientState::ReadResult result =
    client->m_clientState->read(inputBuffer, *client);

  if (result.errorOccurred) {
    client->closeAndDelete();
    return;
  }

  size_t clippedReadSize = std::min(result.readSize, client->m_buffer.size());

  std::cout << "clipping: " << clippedReadSize << std::endl;

  client->m_buffer.erase(client->m_buffer.begin(),
                         client->m_buffer.begin() + clippedReadSize);

  client->m_readSize -= clippedReadSize;
}

void
Client::setState(std::unique_ptr<ClientState>&& state)
{
  m_clientState = std::move(state);
}

bool
Client::sendRawData(std::string&& data)
{
  WriteOperation* writeOp = WriteOperation::create(std::move(data));

  return writeOp->execute((uv_stream_t*)&m_client);
}

std::unique_ptr<Security>
Client::createDefaultSecurity()
{
  return std::unique_ptr<Security>(new NullSecurity());
}

} // namespace vncs
