#include "unittypedata.h"

#include "region-methods.h"

namespace sakusen {

UnitTypeData::UnitTypeData(
    HitPoints mHP,
    uint8 m,
    const Point<uint32>& s,
    const Region<sint16>& pA,
    const Region<sint16>& pV,
    const Region<sint16>& pAV,
    const Visibility& visib,
    const Sensors& visio
  ) :
  maxHitPoints(mHP),
  mass(m),
  size(s),
  possibleAccelerations(pA),
  possibleVelocities(pV),
  possibleAngularVelocities(pAV),
  visibility(visib),
  vision(visio)
{
}

UnitTypeData::UnitTypeData(const IUnitTypeData* copy) :
  maxHitPoints(copy->getMaxHitPoints()),
  mass(copy->getMass()),
  size(copy->getSize()),
  possibleAccelerations(copy->getPossibleAccelerations()),
  possibleVelocities(copy->getPossibleVelocities()),
  possibleAngularVelocities(copy->getPossibleAngularVelocities()),
  visibility(copy->getVisibility()),
  vision(copy->getVision())
{
}

void UnitTypeData::store(OArchive& out) const
{
  out << maxHitPoints;
  out << mass;
  out << size;
  possibleAccelerations.store(out);
  possibleVelocities.store(out);
  possibleAngularVelocities.store(out);
  visibility.store(out);
  vision.store(out);
}

UnitTypeData UnitTypeData::load(IArchive& in)
{
  HitPoints maxHitPoints;
  uint8 mass;
  Point<uint32> size;
  in >> maxHitPoints >> mass >> size;
  Region<sint16> possibleAccelerations = Region<sint16>::load(in);
  Region<sint16> possibleVelocities = Region<sint16>::load(in);
  Region<sint16> possibleAngularVelocities = Region<sint16>::load(in);
  Visibility visibility(in);
  Sensors vision(in);

  return UnitTypeData(
      maxHitPoints, mass, size,
      possibleAccelerations, possibleVelocities, possibleAngularVelocities,
      visibility, vision
    );
}

}
