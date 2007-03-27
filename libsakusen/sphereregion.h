#ifndef SPHEREREGION_H
#define SPHEREREGION_H

#include "libsakusen-global.h"
#include "region.h"
#include "world.h"
#include "mathsutils.h"

namespace sakusen {

/** \brief Describes a region which is spherical */
template<typename T>
class LIBSAKUSEN_API SphereRegion : public Region<T> {
  friend class Region<T>;
  /* Templatey typedefs with macros to spell them out in full for SWIG's
   * benefit
   */
#ifdef SWIG
#define UT IntMunger<T>::unsign
#define UWideT IntMunger<IntMunger<T>::unsign>::widest
#else
    typedef typename IntMunger<T>::unsign UT;
    typedef typename IntMunger<UT>::widest UWideT;
#endif
  public:
    SphereRegion() : centre(), radius(0) {}
    SphereRegion(const Point<T>& c, UT r) :
      centre(c), radius(r) {}
  private:
    Point<T> centre;
    UT radius;

    void storeData(OArchive& archive) const
    {
      archive << centre << radius;
    }
    
    static SphereRegion<T>* loadNew(IArchive& archive)
    {
      Point<T> centre;
      UT radius;
      archive >> centre >> radius;
      return new SphereRegion<T>(centre, radius);
    }
  public:
    inline UWideT squareRadius() const { return UWideT(radius) * radius; }
    inline bool contains(const Point<T>& point) const;
    inline Point<T> truncateToFit(const Point<T>&) const;
    inline Point<T> getBestPosition() const { return centre; }
    inline Rectangle<T> getBoundingRectangle() const;
    inline Box<T> getBoundingBox() const;
    inline double intersect(const Ray&) const;
    
    RegionType getType() const { return regionType_sphere; }
};

template<typename T>
inline bool SphereRegion<T>::contains(const Point<T>& point) const {
    return world->getMap()->getShortestDifference(point, centre).
    squareLength() < squareRadius();
}

/** \bug This won't work if the sphere is not centred on the origin. I'm not
 * sure what to do about this. It looks like we would only ever want to use
 * this on the origin, and code to make it work anywhere would be just enough
 * of a speed penalty to make me unhappy.
 */
template<typename T>
inline Point<T> SphereRegion<T>::truncateToFit(
    const Point<T>& p
  ) const
{
  if (contains(p)) {
    return p;
  }
  return Point<T>((Point<double>(p * radius) / p.length()).truncate());
}

template<typename T>
inline Rectangle<T> SphereRegion<T>::getBoundingRectangle() const
{
  return Rectangle<T>(
      centre.x-radius,
      centre.y-radius,
      centre.x+radius,
      centre.y+radius
    );
}

template<typename T>
inline Box<T> SphereRegion<T>::getBoundingBox() const
{
  return Box<T>(
      Point<T>(
        centre.x-radius,
        centre.y-radius,
        centre.z-radius
      ),
      Point<T>(
        centre.x+radius,
        centre.y+radius,
        centre.z+radius
      )
    );
}

template<typename T>
inline double SphereRegion<T>::intersect(const Ray& r) const
{
  const Position relativeOrigin = r.origin - centre;
  // Set up a quadratic to solve for the distance parameter
  double a = r.d.squareLength();
  double b = 2*r.d.innerProduct(relativeOrigin);
  double c = relativeOrigin.squareLength();

  double r1, r2;
  boost::tie(r1, r2) = mathsUtils_solveQuadratic(a, b, c);
  if (isnan(r1)) {
    return std::numeric_limits<double>::infinity();
  }
  if (r1 > 0) {
    return r1;
  }
  if (r2 > 0) {
    return r2;
  }
  return std::numeric_limits<double>::infinity();
}

#ifdef _MSC_VER
template LIBSAKUSEN_API SphereRegion<sint16>;
#endif

}

#endif // SPHEREREGION_H
