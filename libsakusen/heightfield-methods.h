#ifndef HEIGHTFIELD_METHODS_H
#define HEIGHTFIELD_METHODS_H

#include "heightfield.h"
#include "world.h"

namespace sakusen {

inline uint32 Heightfield::dexToSampleFloorX(sint32 x) const {
  return static_cast<uint32>(x - world->getMap()->left()) /
    horizontalResolution;
}

inline uint32 Heightfield::dexToSampleFloorY(sint32 y) const {
  return static_cast<uint32>(y - world->getMap()->bottom()) /
    horizontalResolution;
}

inline uint32 Heightfield::dexToSampleCeilX(sint32 x) const {
  uint32 offset = static_cast<uint32>(x - world->getMap()->left());
  if (0 == offset) {
    return 0;
  } else {
    return (offset-1)/horizontalResolution + 1;
  }
}

inline uint32 Heightfield::dexToSampleCeilY(sint32 y) const {
  uint32 offset = static_cast<uint32>(y - world->getMap()->bottom());
  if (0 == offset) {
    return 0;
  } else {
    return (offset-1)/horizontalResolution + 1;
  }
}

inline sint32 Heightfield::sampleToDexX(uint32 x) const {
  return world->getMap()->left() + x * horizontalResolution;
}

inline sint32 Heightfield::sampleToDexY(uint32 y) const {
  return world->getMap()->bottom() + y * horizontalResolution;
}

}

#endif // HEIGHTFIELD_METHODS_H
