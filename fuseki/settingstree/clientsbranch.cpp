#include "settingstree/clientsbranch.h"

#include "settingstree/clientbranch.h"

using namespace fuseki::settingsTree;

ClientsBranch::ClientsBranch(const Branch* parent, Server* server) :
  Branch("clients", "world", "", parent, server)
{
}

ClientsBranch::~ClientsBranch()
{
}

void ClientsBranch::addClient(ClientID id)
{
  String name = clientID_toString(id);
  if (getChild(name) != NULL) {
    Fatal("tried to add client branch of existing name " << name);
  }
  addChild(new ClientBranch(id, this, server));
}

void ClientsBranch::removeClient(ClientID id)
{
  String name = clientID_toString(id);
  if (getChild(name) == NULL) {
    Fatal("tried to remove non-existant client branch " << name);
  }
  removeChild(name);
}

