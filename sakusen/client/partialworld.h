#ifndef LIBSAKUSEN_CLIENT__PARTIALWORLD_H
#define LIBSAKUSEN_CLIENT__PARTIALWORLD_H

#include <boost/scoped_ptr.hpp>

#include <sakusen/gnu_extensions.h>
#include <sakusen/hash_list.h>
#include <sakusen/idindex.h>
#include <sakusen/world.h>
#include <sakusen/client/partialmap.h>
#include <sakusen/client/updatedunit.h>
#include <sakusen/client/unitfactory.h>
#include <sakusen/client/sensorreturnsfactory.h>
#include <sakusen/client/updatedsensorreturns.h>
#include <sakusen/client/updatedballistic.h>
#include <sakusen/update.h>
#include <sakusen/rectangle.h>

namespace sakusen {
namespace client {

/** \brief Helps clients keep track of the game state.
 *
 * This class is intended to perform those actions common to all clients
 * required to accurately keep track of the client's perspective on the game.
 * The client should call applyUpdate with each Update it recieves from the
 * server, and then endTick each time it determines that a tick has ended.
 * PartialWorld will automatically interpret this in the correct manner and
 * provides means for the client to examine this information (e.g. a spatial
 * index of the player's units and SensorReturns).
 */
class LIBSAKUSEN_CLIENT_API PartialWorld : public World {
  public:
    PartialWorld(
        const Universe::ConstPtr& universe,
        const UnitFactory*,
        const SensorReturnsFactory*,
        PlayerId playerId,
        Topology topology,
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity,
        const Heightfield&
      );
    ~PartialWorld();

    typedef IdIndex<UnitId, UpdatedUnit, UpdatedUnitIder> UnitIdIndex;
    typedef IdIndex<
        SensorReturnsId, UpdatedSensorReturns, SensorReturnsIder
      > SensorReturnsIdIndex;
    typedef IdIndex<ClientBallisticId, UpdatedBallistic, UpdatedBallisticIder>
      BallisticIdIndex;
  private:
    boost::scoped_ptr<const UnitFactory> unitFactory;
    boost::scoped_ptr<const SensorReturnsFactory> sensorReturnsFactory;
    PlayerId playerId;
    PartialMap* map;
    hash_list<UpdatedUnit, Bounded, UpdatedUnit> units;
    UnitIdIndex::Ptr unitsById;

    hash_list<UpdatedSensorReturns, Bounded, UpdatedSensorReturns>
      sensorReturns;
    SensorReturnsIdIndex::Ptr sensorReturnsById;

    hash_list<UpdatedBallistic, Bounded, UpdatedBallistic> ballistics;
    BallisticIdIndex::Ptr ballisticsById;

    ISpatial::Ptr spatialIndex;

    Ref<ISensorReturns> getISensorReturns(PlayerId player, SensorReturnsId id) {
      assert(player == playerId);
      static_cast<void>(player);
      return sensorReturnsById->find(id);
    }
    Ref<ICompleteUnit> getICompleteUnit(PlayerId player, UnitId uintId) {
      assert(player == playerId);
      static_cast<void>(player);
      return unitsById->find(uintId);
    }
  public:
    /** \name Accessors. */
    //@{
    inline PlayerId getPlayerId() const { return playerId; }
    inline sakusen::Map* getMap() { return map; }
    inline const sakusen::Map* getMap() const { return map; }
    inline UnitIdIndex::ConstPtr getUnitsById() const {
      return unitsById;
    }
    inline SensorReturnsIdIndex::ConstPtr getSensorReturnsById() const {
      return sensorReturnsById;
    }
    inline sakusen::ISpatial::ConstPtr getSpatialIndex() const {
      return spatialIndex;
    }
    inline const PartialMap* getPartialMap() const {
      return map;
    }
    //@}

    void applyUpdate(const Update&);
    void endTick();
};

extern LIBSAKUSEN_CLIENT_API PartialWorld* world;

}}

#endif // LIBSAKUSEN_CLIENT__PARTIALWORLD_H

