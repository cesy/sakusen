#ifndef LIBSAKUSEN__BOUNDED_H
#define LIBSAKUSEN__BOUNDED_H

#include <sakusen/global.h>
#include <sakusen/box.h>
#include <sakusen/gameobject.h>

namespace sakusen {

class Ray;

/** \brief A bounded region of or object in game-space.
 *
 * Any class that has meaningful extents in space should inherit from this
 * abstract class. Usually, subclasses represent tangible objects in the game
 * world, but they could also be abstract regions of game space; for example, a
 * unit's weapon range that the client wants to monitor for the presence of
 * enemy units.
 *
 * It demands methods for finding the bbox of the bounded object and spatially
 * relating with other game objects.
 */
class LIBSAKUSEN_API Bounded {
  public:
    /** \brief Virtual destructor.
     *
     * This destructor does nothing at present but needs to be virtual in case
     * subclasses want to override it.
     */
    virtual ~Bounded() {}

    /** \brief Retrieve object type.
     *
     * Find the objects type so it can be discarded quickly without computing
     * the bounding box in situations where that is beneficial. */
    virtual GameObject getObjectType() const = 0;

    /** \brief Retrieve the object's bbox.
     *
     * Really, you shouldn't call this method very often. If you want to do
     * operations on the bbox, you should edit this class to add operations
     * that delegate to the bbox.
     *
     * This method is pure virtual: subclasses should override it with the
     * appropriate code.
     */
    virtual Box<sint32> getBoundingBox(void) const = 0;

    /** \brief Intersect a Ray with the bbox.
     *
     * By default, this function calls getBoundingBox() and intersects the Ray
     * with the result, but it is left virtual so that subclasses can override
     * it (in case the actual intersection test is at least as fast as the bbox
     * intersection test, as for spheres).
     *
     * \param[in] r The Ray to trace.
     * \return The distance parameters for the ray when it hits the boundary of
     * the bbox, or (+Inf,+Inf) if there are no intersections. One or both
     * values may be negative.
     */
    virtual boost::tuple<double,double> fastIntersect(const Ray& r) const;

    /** \brief Intersect a Ray with the actual object.
     *
     * This pure virtual method should be overridden by subclasses with the
     * appropriate code to return the first intersection of the ray with the
     * actual object bounded by the box.
     *
     * \param[in] r The Ray to trace.
     * \return The distance parameters for the ray when it hits the boundary of
     * the object, or (+Inf,+Inf) if there are no intersections.  If the only
     * intersections are for negative distance parameters, then it is
     * permissible to return (+Inf,+Inf).  If the first would be negative then
     * it is permissible to replace it with -Inf (e.g. if that is faster than
     * calculating it exactly).
     */
    virtual boost::tuple<double,double> intersect(const Ray& r) const = 0;

    /** \brief Determine whether the bbox contains a given point.
     *
     * By default, this calls getBoundingBox() and determines whether that
     * contains the given point, but it is left virtual so that subclasses can
     * override it (in case the actual test is at least as fast as the bbox
     * test).
     *
     * \param p The position to test.
     * \return true iff p lies in the bbox.
     */
    virtual bool fastContains(const Position& p) const;

    /** \brief Determine whether the object contains a given point.
     *
     * This pure virtual method should be overriden by subclasses with the
     * appropriate code to determine whether the actual object contains the
     * given point.
     * \param p The position to test.
     * \return true iff p lies in the bbox.
     */
    virtual bool contains(const Position& p) const = 0;

    /** \brief Intersect this bbox with another.
     *
     * This method decides whether the intersection of the regions enclosed by
     * the two bboxes is non-zero. That is, it will say yes iff the boundaries
     * overlap or one is completely enclosed by the other. It cannot be
     * overridden because the subclass would need to know the details of both
     * objects to do it sensibly.
     *
     * It is named "fast" because it operates only on the bboxes rather than
     * the bounded objects, even though there is no corresponding "slow"
     * operation in this case.
     *
     * \param[in] b The bounded object whose bbox should be intersected with
     * this one.
     * \return true iff the two bboxes intersect.
     */
    bool fastIntersection(const Bounded& b) const;

    /** \brief Intersect this bbox with a box.
     *
     * This method decides whether the intersection of the regions enclosed by
     * the two boxes is non-zero. That is, it will say yes iff the boundaries
     * overlap or one is completely enclosed by the other. It cannot be
     * overridden because the subclass would need to know the details of both
     * objects to do it sensibly.
     *
     * It is named "fast" because it operates only on the bboxes rather than
     * the bounded objects, even though there is no corresponding "slow"
     * operation in this case.
     *
     * \param[in] b The box which should be intersected withthe bounding box
     *  of this one.
     * \return true iff the two boxes intersect.
     */
    bool fastIntersection(const Box<sint32>& b) const;
};

}

#endif
