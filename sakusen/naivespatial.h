#ifndef LIBSAKUSEN__NAIVESPATIAL_H
#define LIBSAKUSEN__NAIVESPATIAL_H

#include <sakusen/ispatial.h>

namespace sakusen {
/** \brief Naive implementation of spatial index
 *
 * This simple implementation of a spatial index is intended to serve as a
 * reference against which more efficient indices can be compared */
class LIBSAKUSEN_API NaiveSpatial : public ISpatial {
  private:
    typedef std::list<Ref<Bounded> > List;
    List list;
    u_map<MaskedPtr<Bounded>, List::iterator>::type iteratorMap;
  public:
    void add(const Ref<Bounded>&);
    void remove(const Ref<Bounded>&);

    Result findIntersecting(
        const Box<sint32>&,
        const GameObject filter
      ) const;
    using ISpatial::findIntersecting;
    Result findFastContaining(
        const Position&,
        const GameObject filter
      ) const;
    Result findContaining(
        const Position&,
        const GameObject filter
      ) const;
    std::set<Intersection, LessThanIntersectionPosition> findIntersections(
        const Ray&,
        const double extent,
        const GameObject filter
      ) const;
};

}

#endif // LIBSAKUSEN__NAIVESPATIAL_H

