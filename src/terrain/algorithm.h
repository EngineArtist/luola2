#ifndef LUOLA_TERRAIN_ALGORITHM_H
#define LUOLA_TERRAIN_ALGORITHM_H

#include "common.h"

namespace terrain {
namespace algorithm {

    class GeometryException : public std::exception {
    public:
        GeometryException(const char *msg) : m_error(msg) { }
        const char *what() const throw() { return m_error; }
    private:
        const char *m_error;
    };

    /**
     * Check if the point P is inside the triangle ABC
     *
     * @param A triangle endpoint
     * @param B triangle endpoint
     * @param C triangle endpoint
     * @param P point to check
     * @return true if point is inside the triangle
     */
    bool pointInTriangle(const Point &A, const Point &B, const Point &C, const Point &P);

    /**
     * Triangulate a polygon
     *
     * @param polygon a list of points defining a polygon.
     * @return list of triangles
     */
     std::vector<Points> triangulate(const Points &polygon);

     /**
      * Partition the given polygon into convex polygons.
      *
      * A fast algorithm that is not guaranteed to return
      * the optimal partitioning is used.
      *
      * @param polygon a list of points defining a polygon
      * @return list of convex polygons
      */
     std::vector<Points> fastPartition(const Points &polygon);

     /**
      * Check if the line segments (a1,a2) and (b1,b2) intersect.
      *
      * If the lines intersect, p will be set to the intersection
      * point and true will be returned.
      *
      * @param a1 first vertex of line 1
      * @param a2 second vertex of line 1
      * @param b1 first vertex of line 2
      * @param b2 second vertex of line 2
      * @param p intersection point
      * @return true if lines intersect.
      */
     bool lineIntersection(const Point &a1, const Point &a2, const Point &b1, const Point &b2, Point &p);
}
}

#endif

