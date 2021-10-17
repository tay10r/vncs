#pragma once

#include <set>

namespace vncs {

class Client;

class ClientList final
{
public:
  void add(Client* client);

  void makeExclusive(Client* client);

  void remove(Client* client);

private:
  std::set<Client*> m_clientPointers;
};

} // namespace vncs
