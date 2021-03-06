#include <sakusen/weapontype.h>

using namespace sakusen;

WeaponType::WeaponType(
    String iN,
    String mN,
    String cH,
    uint16 eC,
    uint16 mC,
    uint16 eR,
    uint16 mR,
    ResourceInterface::Ptr resourceInterface
  ) :
  internalName(iN),
  moduleName(mN),
  clientHint(cH),
  spawnFunction(NULL),
  energyCost(eC),
  metalCost(mC),
  energyRate(eR),
  metalRate(mR)
{
  ResourceSearchResult result;
  boost::tie(spawnFunction, result) = resourceInterface->
    symbolSearch<server::Weapon* (*)(const WeaponType*)>(
        moduleName, "spawn_"+internalName
      );
  switch (result)
  {
    case resourceSearchResult_notSupported:
      /* This means we are a client and don't need the thing anyway; we
       * paranoically assign NULL again. */
      spawnFunction = NULL;
      return;
    case resourceSearchResult_success:
      /* Everything is OK */
      return;
    default:
      /* There is a problem */
      throw ResourceDeserializationExn(
          moduleName, result, resourceInterface->getError()
        );
  }
}

/** \brief Spawn a weapon of this type which must be deleted by the caller */
server::Weapon* WeaponType::spawn() const
{
  if (spawnFunction == NULL) {
    SAKUSEN_FATAL("missing spawn function");
  }

  return (*spawnFunction)(this);
}

void WeaponType::store(OArchive& archive) const
{
  archive << internalName << moduleName << clientHint << energyCost <<
    metalCost << energyRate << metalRate;
}

WeaponType WeaponType::load(
    IArchive& archive,
    const DeserializationContext& context
  )
{
  String internalName;
  String moduleName;
  String clientHint;
  uint16 energyCost;
  uint16 metalCost;
  uint16 energyRate;
  uint16 metalRate;
  archive >> internalName >> moduleName >> clientHint >> energyCost >>
    metalCost >> energyRate >> metalRate;
  return WeaponType(
      internalName, moduleName, clientHint, energyCost, metalCost,
      energyRate, metalRate, context.getResourceInterface()
    );
}

