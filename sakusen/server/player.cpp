#include <sakusen/server/player.h>
#include <sakusen/updatedata.h>
#include <sakusen/server/layeredunit.h>
#include <sakusen/server/completeworld.h>

#include <stack>

using namespace std;

namespace sakusen{
namespace server{

Player::Player(const PlayerTemplate& t, const PlayerId i) :
  MaterielProvider(),
  noClients(t.isNoClients()),
  raceFixed(t.isRaceFixed()),
  playerId(i),
  name(""),
  clients(),
  units(),
  nextUnitId(),
  sensorReturns(),
  sensorReturnsById(new SensorReturnsIdIndex()),
  nextSensorReturnsId(),
  visibleBallistics(),
  nextClientBallisticId(),
  availableEnergy(0),
  availableMetal(0),
  playerData()
{
  assert(raceFixed || !noClients);
  sensorReturns.registerIndex<DynamicSensorReturns>(sensorReturnsById);
}

Player::Player(const Player& copy) :
  MaterielProvider(copy),
  noClients(copy.noClients),
  raceFixed(copy.raceFixed),
  playerId(copy.playerId),
  name(copy.name),
  clients(copy.clients),
  units(copy.units),
  nextUnitId(copy.nextUnitId),
  sensorReturns(),
  sensorReturnsById(new SensorReturnsIdIndex()),
  nextSensorReturnsId(copy.nextSensorReturnsId),
  visibleBallistics(copy.visibleBallistics),
  nextClientBallisticId(copy.nextClientBallisticId),
  availableEnergy(copy.availableEnergy),
  availableMetal(copy.availableMetal),
  playerData(copy.playerData)
{
  if (!copy.sensorReturns.empty()) {
    /* To perform this copy we'd need to set up sensorReturnsById, which would
     * be non-trivial */
    SAKUSEN_FATAL("Don't support copying a player with SensorReturns in place");
  }
  sensorReturns.registerIndex<DynamicSensorReturns>(sensorReturnsById);
}

Player& Player::operator=(const Player& copy)
{
  if (!copy.sensorReturns.empty()) {
    /* To perform this copy we'd need to set up sensorReturnsById, which would
     * be non-trivial */
    SAKUSEN_FATAL("Don't support copying a player with SensorReturns in place");
  }

  noClients = copy.noClients;
  raceFixed = copy.raceFixed;
  playerId = copy.playerId;
  name = copy.name;
  clients = copy.clients;
  units = copy.units;
  nextUnitId = copy.nextUnitId;
  sensorReturns.clear();
  nextSensorReturnsId = copy.nextSensorReturnsId;
  visibleBallistics = copy.visibleBallistics;
  nextClientBallisticId = copy.nextClientBallisticId;
  availableEnergy = copy.availableEnergy;
  availableMetal = copy.availableMetal;
  playerData = copy.playerData;

  return *this;
}

Player::~Player()
{
}

void Player::attachClient(Client* client)
{
  assert(!noClients);
  if (clients.end() != find(clients.begin(), clients.end(), client)) {
    SAKUSEN_DEBUG("Tried to attach an already attached client");
    return;
  }
  clients.push_back(client);
}

void Player::detachClient(Client* client)
{
  assert(!noClients);
  std::list<Client*>::iterator c = find(clients.begin(), clients.end(), client);
  if (c != clients.end()) {
    clients.erase(c);
    return;
  }

  SAKUSEN_DEBUG("Tried to detach an unattached client");
}

uint32 Player::requestEnergy(uint32 amount) {
  uint32 result = static_cast<uint32>(std::min<uint64>(amount, availableEnergy));
  availableEnergy -= result;
  return result;
}

uint32 Player::requestMetal(uint32 amount) {
  uint32 result = static_cast<uint32>(std::min<uint64>(amount, availableMetal));
  availableMetal -= result;
  return result;
}

/** \brief Remove a unit from this player.
 *
 * For internal use only.
 *
 * Any call to removeUnit must be followed by a call to addUnit for
 * another player, otherwise the unit will end up in limbo. */
void Player::removeUnit(const UnitId id, enum changeOwnerReason why)
{
  u_map<UnitId, Ref<LayeredUnit> >::type::iterator unit =
    units.find(id);
  if (unit == units.end()) {
    SAKUSEN_FATAL("tried to remove a unit which wasn't there");
  }
  /*SAKUSEN_DEBUG("removing unit " << id << " for player " << playerId);*/
  units.erase(id);
  informClients(Update(new UnitRemovedUpdateData(id, why)));
}

void Player::addUnit(const Ref<LayeredUnit>& unit, enum changeOwnerReason why)
{
  /*SAKUSEN_QDEBUG("[Player::addUnit]");*/
  if (units.count(nextUnitId)) {
    SAKUSEN_FATAL("tried to insert a unit with an existing id");
  }
  units[nextUnitId] = unit;
  assert(units.find(nextUnitId) != units.end());
  unit->setId(nextUnitId);
  informClients(Update(new UnitAddedUpdateData(why, unit)));
  ++nextUnitId;
}

/** \brief Update all player's sensor returns for a new tick.
 *
 * This function will add, remove and update sensor returns of this player as
 * appropriate, performing all the necessary visibility checks and so on.
 *
 * Most of the server's CPU time is spent inside this function.
 */
void Player::checkSensorReturns()
{
  /* check over our sensor returns for ones where the units have been destroyed
   * */
  for (DynamicSensorReturnsRef returns = sensorReturns.begin();
      returns != sensorReturns.end(); ) {
    /* The fact that the SensorReturns has become empty without our calling
     * update should be enough to prove that the unit is gone */
    if ((*returns)->empty()) {
      assert((*returns)->getPerception() == perception_none);
      informClients(
          Update(new SensorReturnsRemovedUpdateData((*returns)->getId()))
        );
      returns = sensorReturns.erase(returns);
    } else {
      ++returns;
    }
  }

  /* First we deal with other players' units */
  for (hash_list<LayeredUnit, Bounded>::iterator unit =
      world->getUnits().begin(), end = world->getUnits().end();
      unit != end; ++unit) {
    /* check whether this unit already has a return to this player */
    u_map<PlayerId, DynamicSensorReturnsRef>::type::iterator it =
      (*unit)->getSensorReturns().find(playerId);
    if (it != (*unit)->getSensorReturns().end()) {
      /* we have a sensor return, so we update it */
      Ref<DynamicSensorReturns> returns = *it->second;
      returns->update();
      /* If it's gone, then remove it */
      if (returns->empty()) {
        informClients(
            Update(new SensorReturnsRemovedUpdateData(returns->getId()))
          );
        sensorReturns.erase(*it->second);
        (*unit)->getSensorReturns().erase(it);
      } else if (returns->isDirty()) {
        /* It's changed, so inform clients */
        informClients(Update(new SensorReturnsAlteredUpdateData(returns)));
        returns->clearDirty();
      }
    } else if ((*unit)->getOwner() != playerId) {
      /* we have no sensor return, so we determine whether one should exist
       * */
      SensorReturnsId newId = nextSensorReturnsId;
      DynamicSensorReturns newReturns(newId, *unit, this);
      if (!newReturns.empty()) {
        /* Clear the dirty flag that will certainly have been set */
        newReturns.clearDirty();
        DynamicSensorReturnsRef inserted =
          sensorReturns.push_back(new DynamicSensorReturns(newReturns));
        (*unit)->getSensorReturns().insert(
              pair<PlayerId, DynamicSensorReturnsRef>(playerId, inserted)
            );
        informClients(
            Update(new SensorReturnsAddedUpdateData(*inserted))
          );
        ++nextSensorReturnsId;
      }
    }
  }

  /* First we check over the existing visible Ballistics to remove the ones
   * that have invalidated */
  stack<MaskedPtr<Ballistic> > invalidatedBallisticIds;
  for (u_map<
        MaskedPtr<Ballistic>, pair<ClientBallisticId, Ref<const Ballistic> >
      >::type::iterator
      vb = visibleBallistics.begin(); vb != visibleBallistics.end(); ++vb) {
    if (!vb->second.second) {
      invalidatedBallisticIds.push(vb->first);
      informClients(Update(new BallisticRemovedUpdateData(vb->second.first)));
    }
  }

  /* Now we actually erase the invalidated entries (we couldn't do this before
   * because erasure in a hash_map invalidates iterators) */
  while (!invalidatedBallisticIds.empty()) {
    visibleBallistics.erase(invalidatedBallisticIds.top());
    invalidatedBallisticIds.pop();
  }

  /* Now we check for newly visible Ballistics (of all players) */
  for (hash_list<Ballistic>::const_iterator ballistic =
        world->getBallistics().begin();
      ballistic != world->getBallistics().end(); ++ballistic) {
    if (visibleBallistics.count(*ballistic)) {
      /* we have it already */
      /** \todo See whether it's still visible */
    } else {
      /* It's a new one */
      /** \todo Make an actual visibilty check */
      ClientBallisticId clientId = nextClientBallisticId++;
      /** \bug There's no wraparound check for this id */
      visibleBallistics[*ballistic] =
        pair<ClientBallisticId, Ref<const Ballistic> >(
            clientId, *ballistic
          );
      informClients(Update(
            new BallisticAddedUpdateData(clientId, (*ballistic)->getPath())
          ));
    }
  }
}

/** \brief Process pending orders
 *
 * Gets the orders received from each client and applies them to update
 * unit order queues appropriately */
void Player::applyIncomingOrders(void)
{
  for (std::list<Client*>::iterator client = clients.begin();
      client != clients.end(); client++) {
    while (!(*client)->orderMessageQueueEmpty()) {
      const OrderMessage message = (*client)->orderMessageQueuePopFront();
      u_map<UnitId, Ref<LayeredUnit> >::type::iterator orderee =
        units.find(message.getOrderee());
      if (orderee == units.end()) {
        SAKUSEN_DEBUG("Order for non-existent unit id " << message.getOrderee() <<
              ", player " << playerId);
      } else {
        /*SAKUSEN_DEBUG("order for unit id " << message.getOrderee());*/
        orderee->second->acceptOrder(
            message.getOrder()
          );
      }
    }
  }
}

/** \brief Dispatch an update to all this player's clients */
void Player::informClients(const Update& update)
{
  /*SAKUSEN_DEBUG("clients.size()=" << clients.size());*/
  for (std::list<Client*>::iterator client = clients.begin();
      client != clients.end(); client++) {
    /* SAKUSEN_DEBUG("informing client"); */
    (*client)->queueUpdate(update);
  }
}

}}

