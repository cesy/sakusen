#ifndef LIBSAKUSEN__UNITSTATUS_H
#define LIBSAKUSEN__UNITSTATUS_H

#include "libsakusen-global.h"

#include "iunitstatus.h"
#include "universe.h"

namespace sakusen {

/** \brief Encapsulates status information about a unit.
 *
 * See \ref units for more details.
 */
class LIBSAKUSEN_API UnitStatus : public IUnitStatus {
  public:
    UnitStatus(const IUnitStatus& copy);
    UnitStatus(
      UnitTypeId startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity,
      HitPoints startHitPoints,
      bool startRadarActive,
      bool startSonarActive,
      const std::vector<WeaponStatus>& startWeaponsStatus
    );
    UnitStatus(
      UnitTypeId startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity,
      const HitPoints startHP
    ); /**< This is a simpler version of the above constructor for when you
         don't need to specify the extra data */
    UnitStatus(
      UnitTypeId startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity
    ); /**< This is an even simpler version of the above constructor and will
         set the starting hit points from the default for that unit type. */
    UnitStatus(
      const Universe::ConstPtr& universe,
      UnitTypeId startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity
    ); /**< This is a variant of the above constructor for when the world does
          not exist yet, so the universe must be provided directly */
  public:
    UnitTypeId type;
    /** \name Unit's physical attributes */
    //@{
    Position position;
    Orientation orientation;
    Velocity velocity;
    //@}
    
    /** \name Unit's status */
    //@{
    HitPoints hitPoints;
    bool radarIsActive;
    bool sonarIsActive;
    //@}
    std::vector<WeaponStatus> weaponsStatus;

    void initializeWeapons(const UnitType* typePtr);
  public:
    /* accessors */
    inline UnitTypeId getType(void) const {return type;}
    inline const UnitType* getTypePtr(void) const;
    inline const Point<sint32>& getPosition(void) const {return position;}
    inline const Orientation& getOrientation(void) const {return orientation;}
    inline const Point<sint16>& getVelocity(void) const {return velocity;}
    inline HitPoints getHitPoints(void) const {return hitPoints;}
    inline bool isRadarActive(void) const {return radarIsActive;}
    inline bool isSonarActive(void) const {return sonarIsActive;}
    inline const std::vector<WeaponStatus>& getWeaponsStatus(void) const {
      return weaponsStatus;
    }
    inline std::vector<WeaponStatus>& getWeaponsStatus(void) {
      return weaponsStatus;
    }

    void store(OArchive&, Universe::ConstPtr const&) const;
    static UnitStatus load(IArchive&, const DeserializationContext&);
};

}

#endif // LIBSAKUSEN__UNITSTATUS_H

