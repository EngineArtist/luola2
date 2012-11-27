//
// This file is part of Luola2.
//
// Luola2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Luola2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Luola2.  If not, see <http://www.gnu.org/licenses/>.
//
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

