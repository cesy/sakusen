#include "ballistic.h"
#include "completeworld.h"

using namespace sakusen::server;

Ballistic::Ballistic(
      Time startTime,
      Point<sint32> startPosition,
      Point<sint32> startVelocity) :
  path(startPosition, startVelocity, startTime)
{
}

Ballistic::Ballistic(Quadratic p) :
  path(p)
{
}

/** \brief Check for collisions of this Ballistic over the last tick
 *
 * \return true iff the Ballistic should be removed */
bool Ballistic::resolveIntersections()
{
  /** \bug For now we just check for intersections at the present position, but
   * really we need to check over the path traced over the last tick (linear
   * interpolation should be fine) */

  Point<sint32> position = getPosition(world->getTimeNow());
  /* first we check against the heightfield */
  if (world->getCompleteMap()->getHeightfield().getHeightAt(position) >=
      position.z) {
    onCollision(position);
    return true;
  }

  /** \todo Check for collisions with units */

  return false;
}
