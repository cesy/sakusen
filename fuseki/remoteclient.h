#ifndef REMOTECLIENT_H
#define REMOTECLIENT_H

#include <list>

#include "client.h"
#include "socket.h"
#include "settingsuser.h"
#include "settingstree/leaf.h"

namespace fuseki {

/* Represents a client which is contacted via a socket of some kind (this could
 * be a TCP socket to another machine, or a Unix socket to another process on
 * the same machine, or whatever else is supported) */
class RemoteClient : public sakusen::server::Client, public SettingsUser {
  private:
    RemoteClient();
    RemoteClient(const RemoteClient& copy);
  public:
    RemoteClient(
        sakusen::comms::ClientID id,
        Server* server,
        sakusen::comms::Socket* socket,
        bool abstract
      );
      /* Uses given socket to contact client.  Transfers ownership of socket to
       * this.
       * Uses abstract inSocket iff abstract == true */
    ~RemoteClient();
  private:
    sakusen::comms::ClientID id;
    Server* server;
    sakusen::comms::Socket* inSocket;
    sakusen::comms::Socket* outSocket;
    std::queue<sakusen::comms::Message, std::list<sakusen::comms::Message> > messageQueue;
    bool admin; /* Whether I am an admin */
    bool neverAdmin; /* Whether I never want to be an admin */
    bool ready; /* Whether I am ready to start the game */
    bool autoUnready; /* Whether I want to have my readiness cleared whenever
                         a setting changes */

    void setPlayerId(sakusen::PlayerID id, bool removeGroup);
    inline void setPlayerId(sakusen::PlayerID id) {
      setPlayerId(id, true);
    }
  public:
    /* accessors */
    inline sakusen::comms::ClientID getId() { return id; }
    inline bool isAdmin() { return admin; }
    inline void setAdmin(bool value);
    inline bool isNeverAdmin() { return neverAdmin; }
    inline bool isAutoUnready() { return autoUnready; }

    inline bool isReadyForGameStart() {
      return ready && (observer || (playerId != 0));
    }
    
    void flushIncoming(); /* process all pending messages on inSocket */
    inline bool messageQueueEmpty() const {
      return messageQueue.empty();
    }
    inline sakusen::comms::Message messageQueuePopFront() {
      assert(!messageQueue.empty());
      sakusen::comms::Message message = messageQueue.front();
      messageQueue.pop();
      return message;
    }
    inline void send(const sakusen::comms::MessageData& data) {
      outSocket->send(data);
    }
    void sendUpdate(const sakusen::Update& update);

    /* Perform magic related to the changing value of a client setting */
    String performBoolMagic(
        /*const settingsTree::Leaf* altering,*/
        const std::list<String>& name,
        bool newValue
      );
    template<typename T>
    String performIntMagic(
        /*const settingsTree::Leaf* altering,*/
        const std::list<String>& name,
        T newValue
      );
    String performStringMagic(
        /*const settingsTree::Leaf* altering,*/
        const std::list<String>& name,
        const String& newValue
      );
    String performStringSetMagic(
        /*const settingsTree::Leaf* altering,*/
        const std::list<String>& name,
        const __gnu_cxx::hash_set<String, sakusen::StringHash>& newValue
      );
};

}

#endif // REMOTECLIENT_H

