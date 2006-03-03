#ifndef UI__MAPDISPLAY_H
#define UI__MAPDISPLAY_H

#include "point.h"
#include "game/game.h"
#include "ui/control.h"

namespace tedomari {
namespace ui {

class MapDisplay : public Control {
  private:
    MapDisplay();
    MapDisplay(const MapDisplay&);
  public:
    MapDisplay(
        uint16 x,
        uint16 y,
        DockStyle ds,
        Region* r,
        tedomari::game::Game* g
      ) :
      Control(x, y, ds, r), game(g), posOfDisplayZero(),
      dexPerPixelX(1), dexPerPixelY(1)
    {}
  private:
    tedomari::game::Game* game; /* Not owned by this */
    sakusen::Point<sint32> posOfDisplayZero;
    double dexPerPixelX;
    double dexPerPixelY;

    /* Methods to convert between coordinate systems */
    inline sakusen::Rectangle<double> dexToPixel(
        const sakusen::Rectangle<sint32>& r
      ) const;

    inline sakusen::Rectangle<sint32> pixelToDex(
        const sakusen::Rectangle<double>&
      ) const;
    
    inline sakusen::Point<double> dexToPixel(
        const sakusen::Point<sint32>&
      ) const;
  protected:
    void paint();
  public:
    inline double getDexWidth() const { return dexPerPixelX*getWidth(); }
    inline double getDexHeight() const { return dexPerPixelY*getHeight(); }
    void update();
    void translate(const sakusen::Point<sint32>& d);
    void translate(sint32 dx, sint32 dy);
};

inline sakusen::Rectangle<double> MapDisplay::dexToPixel(
    const sakusen::Rectangle<sint32>& r
  ) const {
  /* Note that the direction of increasing y is reversed during this
   * conversion */
  return sakusen::Rectangle<double>(
      (r.x-posOfDisplayZero.x)/dexPerPixelX,
      (posOfDisplayZero.y-r.y-r.height)/dexPerPixelY,
      r.width/dexPerPixelX,
      r.height/dexPerPixelY
    );
}

inline sakusen::Rectangle<sint32> MapDisplay::pixelToDex(
    const sakusen::Rectangle<double>& r
  ) const {
  /* The following is almost but not quite the smallest inter-aligned
   * rectangle bounding r in game unit space.  It might not be quite large
   * enough (by half a dex) if the lower bound is rounded down and the
   * height/width is not rounded up enough.  However, since a dex is probably
   * much smaller than a pixel, and doing it properly would require more
   * operations, I'm not worried about this small discrepancy */
  return sakusen::Rectangle<sint32>(
      sint32(floor(r.x*dexPerPixelX))+posOfDisplayZero.x,
      posOfDisplayZero.y-sint32(ceil(r.getMaxY()*dexPerPixelY)),
      sint32(ceil(r.width * dexPerPixelX)),
      sint32(ceil(r.height * dexPerPixelY))
    );
}

inline sakusen::Point<double> MapDisplay::dexToPixel(
    const sakusen::Point<sint32>& p
  ) const {
  return sakusen::Point<double>(
      (p.x-posOfDisplayZero.x)/dexPerPixelX,
      (posOfDisplayZero.y-p.y)/dexPerPixelY,
      0
    );
}

}}

#endif // UI__MAPDISPLAY_H

