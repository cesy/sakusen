#ifndef REPLAYRECORDER_H
#define REPLAYRECORDER_H

#include "listener.h"
#include "pluginserverinterface.h"
#include "optioncallback.h"
#include "clientid.h"

namespace sakusen {
namespace server {
namespace plugins {
namespace replay_recorder {

/** \brief Listener which listens to everything and stores it on disk in the
 * form of a replay.
 *
 * */
class ReplayRecorder :
  public Listener, OptionCallback<String>, OptionCallback<uint16> {
  public:
    ReplayRecorder(const PluginServerInterface&);
  private:
    std::vector<std::pair<ClientID, comms::Message> > thisTickMessages;
    String replayFileRoot;
    uint16 keyTickInterval;
    ResourceInterface::Ptr resourceInterface;
    Writer::Ptr mainReplayWriter;
    Writer::Ptr replayIndexWriter;

    void writeKeyTick();
  protected:
    String setOption(const String& name, const String& newVal);
    String setOption(const String& name, const uint16 newVal);
    void gameStart();
    void clientMessage(const Client*, const comms::Message&);
    void ticksMessagesDone();
    void tickDone();
};

}}}}

#endif // REPLAYRECORDER_H
