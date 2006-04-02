#include "icompleteunit.h"

using namespace sakusen;

Rectangle<sint32> ICompleteUnit::getBoundingRectangle(void) const
{
  double dExtents[2];
  uint32 iExtents[2];
  const Point<sint32>& pos = getIStatus()->getPosition();
  const Orientation& orientation = getIStatus()->getOrientation();
  const Point<uint32>& size = getITypeData()->getSize();
  
  for (int i=0; i<2; i++) {
    dExtents[i] = 0;
    for (int j=0; j<3; j++) {
      dExtents[i] += fabs(orientation[i][j])*size[j];
    }
    iExtents[i] = uint32(ceil(dExtents[i]));
  }

  return Rectangle<sint32>(
      pos.x-iExtents[0], pos.y-iExtents[1], pos.x+iExtents[0], pos.y+iExtents[1]
  );
}
