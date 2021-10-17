#include <vncs/client_list.hpp>

#include "client.hpp"

namespace vncs {

void
ClientList::add(Client* client)
{
  m_clientPointers.emplace(client);
}

void
ClientList::makeExclusive(Client* exclusiveClient)
{
  std::set<Client*> listCopy = m_clientPointers;

  for (Client* client : listCopy) {
    if (client == exclusiveClient)
      continue;

    client->closeAndDelete();
  }
}

void
ClientList::remove(Client* client)
{
  m_clientPointers.erase(client);
}

} // namespace vncs
