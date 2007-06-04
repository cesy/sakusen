#ifndef ORDERTYPE_H
#define ORDERTYPE_H

#include <iosfwd>
#include "libsakusen-global.h"

namespace sakusen {

/** \brief Enumeration of types of order which can be give to units.
 *
 * Each type has its own subclass of OrderData which carries the relevant data
 * for that order */
enum OrderType {
  orderType_none,
  /** Set the Unit's velocity to this value */
  orderType_setVelocity,
  /** Move the unit to this position */
  orderType_move,
  /** Target a weapon at a Position */
  orderType_targetPosition,
  /** Target a weapon at a Position & Orientation (e.g. for creating a
   * building with that Position and Orientation) */
  orderType_targetPositionOrientation,
  /** Target a weapon at a unit.  Note that this will always be a friendly unit;
   * enemy units are always SensorReturns */
  orderType_targetUnit,
  /** Target a weapon at a SensorReturns */
  orderType_targetSensorReturns
};

LIBSAKUSEN_API std::ostream& operator<<(std::ostream& output, OrderType);

}

#endif // ORDERTYPE_H

