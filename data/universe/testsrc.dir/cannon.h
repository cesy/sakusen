#ifndef CANNON_H
#define CANNON_H

#include "weapon.h"
#include "ballistic.h"
#include "layeredunit.h"
#include "effect.h"
#include "sphereregiondata.h"
#include "beam.h"

using namespace sakusen;
using namespace sakusen::server;

namespace testsrc {

class Cannon : public Weapon {
  public:
    Cannon(const WeaponType* type) :
      Weapon(type)
    {}
  protected:
    uint32 getProjectileSpeed() const { return 20; }
    void onFire(Ref<LayeredUnit>& firer, uint16 weaponIndex);
  public:
    Weapon* newCopy() const {
      return new Cannon(*this);
    }
};

class Shell : public Ballistic {
  public:
    Shell(Ref<LayeredUnit>& source, const WeaponStatus& status);

    void onCollision(const Point<sint32>& pos);
};

class Explosion : public Effect {
  public:
    Explosion(PlayerID owner, const Point<sint32>& centre, uint32 radius) :
      Effect(
          owner, Region<sint32>(SphereRegionData<sint32>(centre, radius)),
          Visibility()
        ) {}
    void onUnitPresent(Ref<LayeredUnit>&);
};

class Paralyzer : public Weapon {
  public:
    Paralyzer(const WeaponType* type) : Weapon(type) {}
  protected:
    bool aimAt(
        const Ref<LayeredUnit>& firer,
        WeaponStatus* status,
        const Point<sint32>& pos,
        const Point<sint16>& vel
      );
    uint32 getProjectileSpeed() const { return 0; }
    void onFire(Ref<LayeredUnit>& firer, uint16 weaponIndex);
  public:
    Weapon* newCopy() const { return new Paralyzer(*this); }
};

class ParalyzationBeam : public Beam {
  public:
    ParalyzationBeam(Ref<LayeredUnit>& source, const WeaponStatus& status);
};

}

#endif // CANNON_H

