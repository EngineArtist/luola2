#ifndef NDEBUG
#include <iostream>
using std::cerr;
using std::endl;
#endif

#include <stack>
#include <queue>

#include "polygon.h"
#include "algorithm.h"

#include <glm/gtx/norm.hpp>

namespace terrain {

ConvexPolygon::ConvexPolygon(const Points &points)
    : m_points(points), m_bounds(points)
{
    assert(points.size() >= 3);

    // Calculate normals
    for(unsigned int i=1;i<points.size();++i) {
        glm::vec2 n = glm::normalize(m_points[i] - m_points[i-1]);
        m_normals.push_back(glm::vec2(n.y, -n.x));
    }
    glm::vec2 n = glm::normalize(m_points.back() - m_points[0]);
    m_normals.push_back(glm::vec2(-n.y, n.x));
}

void ConvexPolygon::make(const Points &points, std::vector<ConvexPolygon> &polys)
{
#if 1
    for(Points &poly : algorithm::fastPartition(points)) {
        polys.push_back(std::move(poly));
    }
#else
    polys.push_back(points);
#endif
}

bool ConvexPolygon::circleCollision(const Point &p, float r, const glm::vec2 &v, Point &cp, glm::vec2 &normal) const
{
    // constant: how much an object is repositioned off the point of contact
    static const float SMIDGEN = 0.001f;

    for(int i=0;i<vertexCount();++i) {
        // Check if the circle is moving away from the edge
        const glm::vec2 &n = m_normals[i];
        if(glm::dot(v, n) >= 0)
            continue;

        // Leading point on the circle
        Point lead = n * r;
        Point lp0 = p - lead;

        // lp0 - lp1 form the line to test
        Point lp1 = lp0 + v;

        // Check if circle will collide with the edge during this timestep
        const Point &e0 = vertex(i);
        Point icp;
        if(algorithm::lineIntersection(e0, vertex(i+1), lp0, lp1, icp)) {
            // Intersection!
            cp = icp + lead + (n * SMIDGEN);
            normal = n;
            return true;
        }

        // Check if circle will collide with the corner in this timestep
        glm::vec2 w = vertex(i) - p;
        float ww = glm::dot(w, w);
        float a = glm::dot(v, v);
        float b = glm::dot(w, v);
        float c = ww - r*r;

        float root = b*b - a*c;
        if(root >= 0) {
            float t = (-b - glm::sqrt(root)) / a;
            if(t >= 0 && t <= 1) {
                cp = p - v * t - (n * SMIDGEN);
                normal = n;
                return true;
            }
        }
    }

    return false;
}


// Separating Axis Theorem tests
namespace {
    void project_polygon(const ConvexPolygon &polygon, const glm::vec2 &axis, float &min, float &max)
    {
        min = max = glm::dot(polygon.vertex(0), axis);
        for(int i=1;i<polygon.vertexCount();++i) {
            float f = glm::dot(polygon.vertex(i), axis);
            if(f < min)
                min = f;
            if(f > max)
                max = f;
        }
    }

    // Project vertices of p1 and p2 onto axis and return true if they intersect
    bool test_projection(const ConvexPolygon &p1, const ConvexPolygon &p2, const glm::vec2 &axis)
    {
        float p1min, p1max, p2min, p2max;
        project_polygon(p1, axis, p1min, p1max);
        project_polygon(p2, axis, p2min, p2max);

        return p1min < p2max && p1max > p2min;
    }
}

bool ConvexPolygon::hasPoint(const Point &point) const
{
    for(int i=0;i<vertexCount();++i) {
        glm::vec2 side = vertex(i+1) - vertex(i);
        glm::vec2 p = point - vertex(i);
        float k = side.x*p.y - side.y*p.x;
        if(k<0)
            return false;
    }
    return true;
}

bool ConvexPolygon::overlaps(const ConvexPolygon &polygon) const
{
    // First a simple bound check
    if(!bounds().overlaps(polygon.bounds()))
        return false;

    // Overlap check using separating axis theorem
    for(const glm::vec2 &normal : m_normals) {
        if(!test_projection(*this, polygon, normal))
            return false;
    }
    return true;
}

bool ConvexPolygon::envelopes(const ConvexPolygon &polygon) const
{
    for(const Point &p : polygon.vertices())
        if(!hasPoint(p))
            return false;
    return true;
}

namespace {
    /**
     * Find the vertex nearest to the given point.
     *
     * @param points list of vertices
     * @param p the check point
     * @return index of nearest vertex
     */
    int findNearestPoint(const Points &points, const Point &p)
    {
        Point pp = p-points[0];
        float mind = glm::dot(pp, pp);
        int i=0;

        for(unsigned int j=1;j<points.size();++j) {
            pp = p - points[j];
            float d = glm::dot(pp, pp);
            if(d<mind) {
                mind = d;
                i = j;
            }
        }
        return i;
    }

    /**
     * Split a polygon in two halves.
     *
     * The polygon p will be split into two concave polygons (p1, p2) with
     * the hole h left in the middle, sort of like a mold.
     * @param p polygon to split
     * @param h the hole in the middle
     * @param p1 list of vertices for one half
     * @param p2 list of vertices for the other half
     */
    void splitPolygon(const ConvexPolygon &p, const ConvexPolygon &h, Points &p1, Points &p2)
    {
        int pivot = p.vertexCount() / 2;
        int nearest0 = findNearestPoint(h.vertices(), p.vertex(0));
        int nearest1 = findNearestPoint(h.vertices(), p.vertex(pivot));
        int i;

        // First half
        for(i=0;i<=pivot;++i)
            p1.push_back(p.vertex(i));

        i = nearest1;
        while(i!=nearest0) {
            p1.push_back(h.vertex(i));
            if(--i < 0)
                i = h.vertexCount() - 1;
        }
        p1.push_back(h.vertex(i));

        // Second half
        for(i=pivot;i<p.vertexCount();++i)
            p2.push_back(p.vertex(i));
        p2.push_back(p.vertex(0));

        i = nearest0;
        while(i!=nearest1) {
            p2.push_back(h.vertex(i));
            if(--i < 0)
                i = h.vertexCount() - 1;
        }
        p2.push_back(h.vertex(i));
    }

    /**
     * Find the edges on poly2 that intersects with poly1 edge (p1, p2).
     *
     * Two edges may be found. isect[0] will always be the edge
     * nearest to p1. The corresponding iv will be set to -1 if the edge
     * is not found.
     * 
     * @param poly1 the polygon whose edge to test
     * @param p1 first vertex of the edge
     * @param p2 second vertex of the edge
     * @param poly2 the polygon whose edges to test
     * @param isect intersection points
     * @param isectv first vertices of intersecting edges (CCW order)
     */
    void findIntersections(
        const ConvexPolygon &poly1, int p1, int p2,
        const ConvexPolygon &poly2,
        Point isect[2], int isectv[2])
    {
        // Since we are intersecting two convex polygons,
        // there can be at most two intersections for any edge.
        const Point &v1 = poly1.vertex(p1);
        const Point &v2 = poly1.vertex(p2);

        isectv[0] = -1;
        isectv[1] = -1;

        int intersections=0;
        for(int j=0;j<poly2.vertexCount();++j) {
            if(algorithm::lineIntersection(v1, v2, poly2.vertex(j), poly2.vertex(j+1), isect[intersections])) {
                isectv[intersections] = j;
                if(++intersections==2)
                    break;
            }
        }

        // If two intersections are found, make sure isect[0] is the closer one.
        if(intersections>1) {
            if(glm::distance2(v1, isect[0]) > glm::distance2(v1, isect[1])) {
                std::swap(isect[0], isect[1]);
                std::swap(isectv[0], isectv[1]);
            }
        }
    }

    struct TracePoint {
        explicit TracePoint(const Point &p_) : p(p_), inext(-1), inhole(false) { }
        TracePoint(const Point &p_, int inext_) : p(p_), inext(inext_), inhole(false) { }

        // The coordinates of this point
        Point p;

        // Next point on the hole polygon or -1 if this is not an intersection point
        int inext;

        bool inhole;
    };

}

void ConvexPolygon::booleanDifference(const ConvexPolygon &hole, std::vector<ConvexPolygon> &list) const
{
    // Special case: this polygon is entirely swallowed by the hole
    if(hole.envelopes(*this))
        return;

    // Special case: the hole is entirely inside this polygon
    if(envelopes(hole)) {
        Points p1, p2;
        splitPolygon(*this, hole, p1, p2);
        ConvexPolygon::make(p1, list);
        ConvexPolygon::make(p2, list);
        return;
    }

    // Common case: hole intersects with this polygon.
    std::vector<bool> visited(vertexCount());
    std::queue<unsigned int> queue;
    queue.push(0);

    // Perform intersection
    unsigned int oldsize = list.size();
    Points poly;
    while(!queue.empty()) {
        int i = queue.front();
        while(i < vertexCount()) {
            // Stop if we've already been here
            if(visited[i])
                break;
            visited[i] = true;

            // Include point if it is not inside the hole
            bool inhole = hole.hasPoint(vertex(i));
            if(!inhole)
                poly.push_back(vertex(i));

            // Check for intersections
            Point isect[2];
            int isectv[2];
            findIntersections(*this, i, i+1, hole, isect, isectv);

            if(isectv[0] >= 0) {
                // Intersection found: this is the start of a hole,
                // except when this edge started inside the hole.
                poly.push_back(isect[0]);
                if(!inhole) {
                    // Start tracing the hole
                    int j = isectv[0];
                    do {
                        // Check for intersections
                        // The first hole edge may intersect with another edges
                        Point hisect[2];
                        int hisectv[2];
                        findIntersections(hole, j+1, j, *this, hisect, hisectv);

                        // There is always one intersection (the one that got us here)
                        if((j == isectv[0] && hisectv[1] >= 0) || (j != isectv[0] && hisectv[0] >= 0)) {
                            // Pick the intersection that is not the one we came in on
                            Point ip;
                            int iv;
                            if(hisectv[1] < 0 || glm::distance2(hisect[0],isect[0]) > glm::distance(hisect[1],isect[0])) {
                                ip = hisect[0];
                                iv = hisectv[0];
                            } else {
                                ip = hisect[1];
                                iv = hisectv[1];
                            }

                            queue.push(i+1);

                            // Avoid adding duplicate point of origin
                            if(glm::distance2(poly.front(), ip) > 0.0001)
                                poly.push_back(ip);
                            i = iv;
                            break;
                        } else {
                            // No intersections? Just add the hole vertex then
                            poly.push_back(hole.vertex(j));
                        }
 
                        if(--j < 0)
                            j = hole.vertexCount() - 1;
                    } while(j != isectv[0]);
                }
            }
            ++i;
        }

        // Partition the generated polygon into convex polygons
        // and add them to the list.
        if(poly.size() >= 3) {
            try {
                ConvexPolygon::make(poly, list);
            } catch(const algorithm::GeometryException &e) {
                // Bad polygons generated... The algorithm works well
                // enough most of the time, let's just roll back the error.
                int changes = list.size() - oldsize;
#ifndef NDEBUG
                cerr << "booleanDifference error: " << e.what() << " (" << changes << " change(s) rolled back)\n";
#endif
                while(changes-->0)
                    list.pop_back();
                list.push_back(*this);
                return;
            }
        }
        poly.clear();
        queue.pop();
    }
}

void ConvexPolygon::toTriangles(Points &points) const
{
#if 0
    // Debugging: triangles with GL_LINES
    points.push_back(m_points[0]);
    points.push_back(m_points[1]);

    points.push_back(m_points[1]);
    points.push_back(m_points[2]);

    points.push_back(m_points[2]);
    points.push_back(m_points[0]);

    for(unsigned int i=2;i<m_points.size();++i) {
        points.push_back(m_points[0]);
        points.push_back(m_points[i]);
        points.push_back(m_points[i]);
        points.push_back(m_points[i-1]);
    }
#else
    // Debugging: Polygon outline with GL_LINES
    for(unsigned int i=1;i<m_points.size();++i) {
        points.push_back(m_points[i-1]);
        points.push_back(m_points[i]);

        // normal
#if 1
        Point halfp = (m_points[i] - m_points[i-1]) * Point(0.5, 0.5);
        Point np = m_points[i-1] + halfp;
        points.push_back(np);
        points.push_back(np + m_normals[i-1] * Point(0.2, 0.2));
#endif
    }
    points.push_back(m_points.back());
    points.push_back(m_points[0]);

#if 1
    Point halfp = (m_points.back() - m_points[0]) * Point(0.5, 0.5);
    Point np = m_points[0] + halfp;
    points.push_back(np);
    points.push_back(np + m_normals.back() * Point(0.2, 0.2));
#endif

#endif
}

}

