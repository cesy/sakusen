#include <sakusen/server/groundmotion.h>

#include <sakusen/unitstatus-methods.h>
#include <sakusen/unitcorneriterator.h>
#include <sakusen/server/completeworld.h>
#include <sakusen/mathsutils.h>

#include <boost/foreach.hpp>

using namespace std;

namespace sakusen {
namespace server {

void GroundMotion::incrementState(LayeredUnit& unit)
{
  UnitStatus& status = unit.getStatus();
  const UnitOrders& orders = unit.getOrders();
  const IUnitTypeData& typeData = unit.getITypeData();
  const Map* map = world->getMap();
  const IHeightfield& hf = map->getHeightfield();
  ISpatial::ConstPtr spatialIndex = world->getSpatialIndex();

  Velocity expectedVelocity(status.velocity);
  AngularVelocity expectedAngularVelocity(status.angularVelocity);

  /* compute the expected velocity based on the unit's orders */
  switch (orders.getLinearTarget()) {
    case linearTargetType_none:
      break;
    case linearTargetType_velocity: /** \todo use correct frame here */
      if (typeData.getPossibleVelocities()->contains(
            orders.getTargetVelocity()
          )) {
        Point<sint16> desiredVelocity = orders.getTargetVelocity();
        Point<sint16> acceleration = desiredVelocity - status.velocity;
        acceleration =
          typeData.getPossibleAccelerations()->truncateToFit(acceleration);
        expectedVelocity += acceleration;
      }
      break;
    case linearTargetType_position:
      {
        Frame& f = status.getFrame();
        Point<sint32> desiredDirection =
          world->getMap()->getShortestDifference(
              orders.getTargetPosition(), f.getPosition()
            );
        Point<sint16> desiredVelocity(
            typeData.getPossibleVelocities()->truncateToFit(
              f.globalToLocalRelative(desiredDirection)
            )
          );
        Point<sint16> acceleration =
          desiredVelocity - f.globalToLocalRelative(status.velocity);
        acceleration =
          Point<sint16>(f.localToGlobalRelative(
                typeData.getPossibleAccelerations()->
                truncateToFit(acceleration))
              );
        /*if (owner == 1 && unitId == 0) {
          SAKUSEN_DEBUG("[1] desiredVel=" << desiredVelocity <<
              ", acc=" << acceleration);
        }*/

        expectedVelocity += acceleration;
      }
      break;
    default:
      SAKUSEN_FATAL("Unknown linearTargetType '" << orders.getLinearTarget() << "'");
      break;
  }

  /* Compute the expected angular velocity based on the unit's orders.
   * At present we allow infinite angular acceleration.
   */
  switch (orders.getRotationalTarget()) {
    case rotationalTargetType_none:
      break;
    case rotationalTargetType_angularVelocity:
      if (typeData.getPossibleAngularVelocities()->contains(
            orders.getTargetAngularVelocity()
          )) {
        expectedAngularVelocity = orders.getTargetAngularVelocity();
      }
      break;
    case rotationalTargetType_orientation:
      {
        Orientation desiredOrientation = orders.getTargetOrientation();
        Orientation desiredOrientationChange =
          desiredOrientation*status.getFrame().getOrientation().inverse();
        expectedAngularVelocity =
          typeData.getPossibleAngularVelocities()->truncateToFit(
              desiredOrientationChange.getGeneratingAngularVelocity()
            );
      }
      break;
    default:
      SAKUSEN_FATAL("Unknown linearTargetType '" << orders.getLinearTarget() << "'");
      break;
  }

  /* Update the expected velocity to take account of gravity and the ground. */
  /** \todo Cause the unit to rotate appropriately when it's supported only on
   * one side. */
  sint32 heightAboveGround = numeric_limits<sint32>::max();
  for (UnitCornerIterator corner(unit), end; corner != end; ++corner) {
    Position cornerPos = *corner;
    map->resolvePosition(cornerPos, cornerPos);
    heightAboveGround = min(
        heightAboveGround, cornerPos.z - hf.getHeightAt(cornerPos)
      );
  }

  if (heightAboveGround + expectedVelocity.z < 0) {
    /* The unit will end up below the ground, so we need to raise it. */
    /** \bug The
     * way this is done at pesent is quite silly and could easily result in
     * exceptionally high velocities. (This is the same bug that lots of
     * speedruns exploit in other games to achieve high speeds) */
    expectedVelocity.z = -heightAboveGround;
  } else if (heightAboveGround > 0) {
    /* The unit is in the air above the ground, so we need to apply gravity
     * */
    expectedVelocity.z -= world->getMap()->getGravity();
    /* But at the same time ensure that we don't push the unit underground */
    if (expectedVelocity.z < -heightAboveGround) {
      expectedVelocity.z = -heightAboveGround;
    }
  }

  /*if (owner == 1 && unitId == 0) {
    SAKUSEN_DEBUG("[2] heightAboveGround=" << heightAboveGround <<
        ", expectedVelocity=" << expectedVelocity);
  }*/

  /* Next we need to detect potential collisions with other units */
  /** \bug At the moment if we forsee a collision we stop the unit dead;
   * obviously we would rather do some physics.
   * \bug We move each unit in order, so it is possible to determine which was
   * built first from the physics, which leaks information. */

  /* Make copies of everything so that we can simulate the move */
  Frame newFrame = status.frame;
  Velocity newVelocity = expectedVelocity;
  AngularVelocity newAngularVelocity = expectedAngularVelocity;

  /* Simulate the move (note: potentially changes all the arguments) */
  world->getMap()->transform(newFrame, newVelocity, newAngularVelocity);

  /* Find the bounding box for the prospective position */
  Box<sint32> newBounds = newFrame.getBoundingBox(typeData.getSize());

  /* Determine all possible collisions with this new position */
  ISpatial::Result potentialCollisions =
    spatialIndex->findIntersecting(newBounds, gameObject_unit);

  bool collision = false;
  BOOST_FOREACH (const Ref<Bounded>& boundedCollision, potentialCollisions) {
    Ref<LayeredUnit> otherUnit = boundedCollision.dynamicCast<LayeredUnit>();
    /* Skip it if it's me */
    if (*otherUnit == unit) {
      continue;
    }
    /** \todo Skip if there is a subunit relationship */

    const UnitStatus& otherStatus = otherUnit->getStatus();
    const Point<uint32> otherSize = otherUnit->getITypeData().getSize();

    if (mathsUtils_boxesIntersect(
          otherStatus.getFrame(), otherSize,
          status.getFrame()+expectedVelocity, typeData.getSize()
      )) {
      /* We've found a non-trivial intersection, but we don't care about it if
       * there was already an intersection before the move happened (otherwise
       * it would be impossible to prise apart units that have become entagled)
       * */
      if (mathsUtils_boxesIntersect(
            otherStatus.getFrame(), otherSize,
            status.getFrame(), typeData.getSize()
        )) {
        continue;
      }
      collision = true;
      break;
    }
  }

  if (collision) {
    expectedVelocity.zero();
    expectedAngularVelocity.zero();
  }

  /* Now set the velocity to this newly computed value */
  if (expectedVelocity != status.velocity) {
    status.velocity = expectedVelocity;
    unit.setDirty();
  }

  /* ... and the angular velocity */
  if (expectedAngularVelocity != status.angularVelocity) {
    status.angularVelocity = expectedAngularVelocity;
    unit.setDirty();
  }
}

}}

