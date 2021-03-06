#include <sakusen/server/dynamicsensorreturns.h>

#include <sakusen/server/player.h>
#include <sakusen/server/layeredunit.h>
#include <sakusen/sphereregion.h>
#include <sakusen/server/completeworld.h>

#include <stack>

using namespace std;

namespace sakusen { namespace server {

DynamicSensorReturns::DynamicSensorReturns(
    SensorReturnsId i,
    const Ref<const LayeredUnit>& s,
    Player* sO
  ) :
  id(i),
  perception(perception_none),
  region(),
  senserOwner(sO),
  sensers(1),
  sensee(s),
  dirty(false)
{
  update();
}

PlayerId DynamicSensorReturns::getSenserOwner() const
{
  return senserOwner->getId();
}

PlayerId DynamicSensorReturns::getSenseeOwner() const
{
  if (0 != (perception & (perception_owner | perception_unit))) {
    return sensee->getOwner();
  } else {
    return PlayerId::invalid();
  }
}

Region<sint32>::ConstPtr DynamicSensorReturns::getRegion() const
{
  /** \todo Is this if really needed? */
  if (0 != (perception & perception_region)) {
    return region;
  } else {
    return Region<sint32>::ConstPtr();
  }
}

Ref<const ICompleteUnit> DynamicSensorReturns::getUnit() const
{
  if (0 != (perception & perception_unit)) {
    return sensee;
  } else {
    return Ref<const ICompleteUnit>();
  }
}

/** \brief Indicate that the sensee has been destroyed.
 *
 * This is called when the unit being sensed is destroyed.  All internal fields
 * are cleared.
 */
void DynamicSensorReturns::senseeDestroyed() {
  if (perception == perception_none) {
    SAKUSEN_FATAL(
        "Tried to indicate destruction when SensorReturns shouldn't have "
        "existed"
      );
  }

  perception = perception_none;
  region.reset();
  sensers.clear();
  sensee = Ref<const LayeredUnit>();
  setDirty();
}

/** \brief Update the sensor return for a new game state */
void DynamicSensorReturns::update()
{
  /* If the sensee has been destroyed, then we see nothing, and give up now */
  if (!sensee) {
    assert(perception == perception_none);
    assert(sensers.empty());
    return;
  }

  /* If the player is player 0 then they get to see everything (so that
   * observers can see everything) */
  if (senserOwner->getId() == 0) {
    if (perception != perception_unit) {
      perception = perception_unit;
      dirty = true;
    }
    return;
  }

  /* If the player owns the unit, then clear and quit */
  if (sensee->getOwner() == senserOwner->getId()) {
    sensers.clear();
    return;
  }

  perception = perception_none;
  uint32 bestRadius = numeric_limits<uint32>::max();

  /* Check over the sensers, and erase any who no longer exist */
  stack<UnitId> deadUnitIds;
  for (SensorReturnMap::iterator senser = sensers.begin();
      senser != sensers.end(); ++senser) {
    if (!senserOwner->getUnits().count(senser->first)) {
      deadUnitIds.push(senser->first);
    }
  }

  while (!deadUnitIds.empty()) {
    sensers.erase(deadUnitIds.top());
    deadUnitIds.pop();
  }

  /* loop over all units of the player doing the sensing */
  /** \todo Loop only over the nearby units
   * \todo Stop once it is clear that adding more units will add no more
   * information. */
  EachSensorReturn workingCopy;

  for (u_map<UnitId, Ref<LayeredUnit> >::type::const_iterator unitIt =
      senserOwner->getUnits().begin(); unitIt != senserOwner->getUnits().end();
      ++unitIt) {
    /* seek out local unit/sensors corresponding to this unit */
    SensorReturnMap::iterator returnsIt = sensers.find(unitIt->first);
    EachSensorReturn* returns;
    if (returnsIt == sensers.end()) {
      workingCopy.clear();
      returns = &workingCopy;
    } else {
      returns = &returnsIt->second;
    }
    Perception thisPerception;
    unitIt->second->getITypeData().getVision().updateReturns(
        unitIt->second, sensee, *returns, &dirty, &thisPerception, &bestRadius
      );
    if (returnsIt != sensers.end() && thisPerception == perception_none) {
      sensers.erase(returnsIt);
    } else if (returnsIt == sensers.end() &&
        thisPerception != perception_none) {
      sensers[unitIt->first] = *returns;
    }
    perception |= thisPerception;
  }

  if (0 != (perception & perception_region)) {
    /* We need to set dirty every time, because presumably the random component
     * could change every tick, and also we aren't informed by the unit
     * whenever its position changes, so that could have altered too. */
    dirty = true;
    MorphingPerlinVectorField<CompleteWorld::random_engine_type>&
      randomDisplacementField = world->getRandomDisplacementField();
    Position const actualPosition = sensee->getIStatus().getPosition();
    /** We add randomness to the position.  The randomness has a standard
     * deviation (in each coordinate axis) of up to 2^31 / 3, so we scale
     * by bestRadius * 3 / 2^31 to make the standard deviation equal
     * bestRadius.
     *
     * \bug The randomness should really be spherically symmetric.  It's not.
     */
    Position const error(Point<sint64>(
        randomDisplacementField(world->getTimeNow(), actualPosition)
      ) * bestRadius * 3 / (sint64(1)<<31));
    region.reset(new SphereRegion<sint32>(
          actualPosition+error, bestRadius
        ));
  } else {
    region.reset();
  }
}

}}

