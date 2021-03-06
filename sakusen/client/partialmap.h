#ifndef LIBCSAKUSEN_CLIENT__PARTIALMAP_H
#define LIBCSAKUSEN_CLIENT__PARTIALMAP_H

#include <sakusen/client/global.h>
#include <sakusen/maps/planemap.h>
#include <sakusen/client/partialheightfield.h>

namespace sakusen {
namespace client {

/** \brief Manages partial knowledge about a map for a client.
 *
 * This class carries what information a client knows about the map, and should
 * provide for updating this infomration as more information is received from
 * the server (not implemented at time of writing).
 *
 * This will be particularly tricky if we intend to pursue the plan of not
 * informing clients where the map boundaries are in advance.
 */
class LIBSAKUSEN_CLIENT_API PartialMap : public Map {
  public:
    PartialMap(const MapTemplate& t) :
        Map(t),
        heightfield(t.getHeightfield())
    {
      sanityCheck();
    }
    PartialMap(
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity,
        const Heightfield& hf
      ) :
      Map(topRight, bottomLeft, gravity),
      heightfield(hf)
    {
      sanityCheck();
    }
  private:
    PartialHeightfield heightfield;

    void sanityCheck();
  public:
    const IHeightfield& getHeightfield() const {
      return heightfield;
    }
    inline const PartialHeightfield& getPartialHeightfield() const {
      return heightfield;
    }
};

}}

#endif // LIBCSAKUSEN_CLIENT__PARTIALMAP_H

