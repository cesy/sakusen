#include "settingstree/clientbranch.h"

#include "settingstree/applicationbranch.h"
#include "settingstree/boolleaf.h"

using namespace fuseki::settingsTree;

ClientBranch::ClientBranch(ClientID id, const Branch* parent, Server* server) :
  Branch(clientID_toString(id), "world", "", parent, server)
{
  String clientGroup = String("client") + clientID_toString(id);
  addChild(new ApplicationBranch("world", "", clientGroup, this, server));
  addChild(new BoolLeaf("admin", false, "world", "admin", this, server));
  addChild(
      new BoolLeaf("neveradmin", true, "world", clientGroup, this, server)
    );
}

