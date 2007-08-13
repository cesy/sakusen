#include "completemap.h"

using namespace std;

using namespace sakusen;
using namespace sakusen::server;

CompleteMap::CompleteMap(const MapTemplate& t) :
  Map(t),
  heightfield(t.getHeightfield())
{
  sanityCheck();
}

/** \brief Do not call.
 *
 * This ctor only exists for complicated reasons
 * related to the way Map types inherit from one
 * another and the Window DLL-import/-export stuff.
 * It will go away in the future.
 *
 * \todo Remove or fix this ctor.
 */
CompleteMap::CompleteMap(
    const Point<sint32>& topRight,
    const Point<sint32>& bottomLeft,
    uint16 gravity,
    uint32 horizontalHeightfieldRes,
    uint32 verticalHeightfieldRes
    ) :
  Map(topRight, bottomLeft, gravity),
  heightfield(horizontalHeightfieldRes, verticalHeightfieldRes, 1, 1)
{
  Fatal("this method should not be called");
}

CompleteMap::CompleteMap(
    const Point<sint32>& topRight,
    const Point<sint32>& bottomLeft,
    uint16 gravity,
    const Heightfield& h
  ) :
  Map(topRight, bottomLeft, gravity),
  heightfield(h)
{
  sanityCheck();
}

void CompleteMap::sanityCheck()
{
  if (0 != width() % heightfield.getHorizontalResolution() ||
      0 != height() % heightfield.getHorizontalResolution()) {
    Fatal("heightfield resolution not eact factor of map dimensions");
  }
  if (heightfield.getWidth() !=
      1+width()/heightfield.getHorizontalResolution()) {
    Fatal("heightfield width incorrect");
  }
  if (heightfield.getHeight() !=
      1+height()/heightfield.getHorizontalResolution()) {
    Fatal("heightfield height incorrect");
  }
}

Map* CompleteMap::newMap(const MapTemplate& t)
{
  switch (t.getTopology()) {
    case topology_plane:
      return new PlaneMap<CompleteMap>(t);
    /** \todo All the other topologies. */
    default:
      throw DeserializationExn("Invalid topology");
  }
}

