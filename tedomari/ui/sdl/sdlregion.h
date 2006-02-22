#ifndef UI__SDLREGION_H
#define UI__SDLREGION_H

#include "ui/region.h"
#include "ui/sdl/sdlui.h"
#include "ui/sdl/sdllayout.h"

namespace tedomari {
namespace ui {
namespace sdl {

class SDLRegion : public Region {
  private:
    SDLRegion();
    SDLRegion(const SDLRegion&);
  public:
    SDLRegion(
        SDLUI* u,
        uint16 xoffset,
        uint16 yoffset,
        uint16 width,
        uint16 height
      ) :
      Region(width, height), ui(u), x(xoffset), y(yoffset)
    {}
  private:
    SDLUI* ui; /* Not owned by this */
    uint16 x;
    uint16 y;
  public:
    inline Region* newSubRegion(uint16 xoff, uint16 yoff, uint16 w, uint16 h) {
      assert(xoff+w <= getWidth());
      assert(yoff+h <= getHeight());
      return new SDLRegion(ui, x+xoff, y+yoff, w, h);
    }
    inline Layout* newLayout() { return new SDLLayout(ui); }
    void flood(Colour);
    void stroke(double x1, double y1, double x2, double y2, const Colour&);
    void drawText(double x, double y, const String& text, const Colour&);
    void drawText(double x, double y, const Layout* layout);
};

}}}

#endif // UI__SDLREGION_H

