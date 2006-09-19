#ifndef WEAPONORDERS_H
#define WEAPONORDERS_H

#include "order.h"
#include "weapontargettype.h"
#include "ref.h"

namespace sakusen {

class LIBSAKUSEN_API WeaponOrders {
  public:
    WeaponOrders();
  private:
    WeaponOrders(
        WeaponTargetType targetType,
        const Point<sint32>& targetPoint,
        const Ref<ISensorReturns>& targetSensorReturns
      );
    
    WeaponTargetType targetType;
    Point<sint32> targetPoint;
    Ref<ISensorReturns> targetSensorReturns;
  public:
    /** \name accessors */
    /*@{*/
    WeaponTargetType getTargetType() const { return targetType; }
    bool isTargetValid() const;
    Point<sint32> getTargetPosition() const;
    Point<sint16> getTargetVelocity() const;
    /*@}*/
    
    /** \brief Update according to the instructions in the given order */
    void update(const Order& order);

    typedef PlayerID loadArgument;
    void store(OArchive&) const;
    static WeaponOrders load(IArchive&, const loadArgument*);
};

}

#endif // WEAPONORDERS_H

