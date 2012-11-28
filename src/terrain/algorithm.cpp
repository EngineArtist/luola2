//
// This file is part of Luola2.
// Copyright (C) 2012 Calle Laakkonen
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
#ifndef NDEBUG
#include <iostream>
using std::cerr;
using std::endl;
#endif

#include <exception>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#include "algorithm.h"

namespace terrain {
namespace algorithm {

// Triangulation code based on snippet by John W. Ratcliff
// http://www.flipcode.com/archives/Efficient_Polygon_Triangulation.shtml
namespace {

static const float EPSILON=0.0000000001f;

bool triSnip(const Points &contour, int u, int v, int w, int n, const std::vector<int> &V)
{
    int p;

    const Point &A = contour[V[u]];
    const Point &B = contour[V[v]];
    const Point &C = contour[V[w]];

    if( EPSILON > (((B.x-A.x) * (C.y-A.y)) - ((B.y-A.y) * (C.x-A.x))))
        return false;

    for (p=0;p<n;p++)
    {
        if((p == u) || (p == v) || (p == w))
            continue;

        if (pointInTriangle(A,B,C,contour[V[p]]))
            return false;
    }

    return true;
}

std::vector<int> triProcess(const Points &contour)
{
    /* allocate and initialize list of Vertices in polygon */
    int n = contour.size();
    if(n < 3)
        throw GeometryException("polygon has less than 3 points");

    std::vector<int> result;
    std::vector<int> V(n);

    for(int v=0; v<n; v++)
        V[v] = v;

    int nv = n;

    /*  remove nv-2 Vertices, creating 1 triangle every time */
    int count = 2*nv;   /* error detection */

    for(int m=0, v=nv-1; nv>2; )
    {
        /* if we loop, it is probably a non-simple polygon */
        if (0 >= (count--)) {
            //** Triangulate: ERROR - probable bad polygon!
            throw GeometryException("loop encountered while triangulating polygon!");
        }

        /* three consecutive vertices in current polygon, <u,v,w> */
        int u = v  ;
        if (nv <= u)
            u = 0;     /* previous */

        v = u+1;
        if (nv <= v)
            v = 0;     /* new v    */

        int w = v+1;
        if (nv <= w)
            w = 0;     /* next     */

        if(triSnip(contour, u, v, w, nv, V)) {
            /* output Triangle */
            result.push_back(V[u]);
            result.push_back(V[v]);
            result.push_back(V[w]);

            m++;

            /* remove v from remaining polygon */
            for(int s=v,t=v+1;t<nv;s++,t++)
                V[s] = V[t];
            nv--;

            /* resest error detection counter */
            count = 2*nv;
        }
    }

    return result;
}

}

bool pointInTriangle(const Point &A, const Point &B, const Point &C, const Point &P)
{
    Point a = C - B;
    Point b = A - C;
    Point c = B - A;

    Point ap = P - A;
    Point bp = P - B;
    Point cp = P - C;

    float aXbp = a.x*bp.y - a.y*bp.x;
    float cXap = c.x*ap.y - c.y*ap.x;
    float bXcp = b.x*cp.y - b.y*cp.x;

    return ((aXbp >= 0.0f) && (bXcp >= 0.0f) && (cXap >= 0.0f));
}

std::vector<Points> triangulate(const Points &polygon)
{
    std::vector<Points> out;
    std::vector<int> triangles = triProcess(polygon);

    for(unsigned int i=0;i<triangles.size();i+=3) {
        Points tri;
        tri.push_back(polygon[triangles[i]]);
        tri.push_back(polygon[triangles[i+1]]);
        tri.push_back(polygon[triangles[i+2]]);
        out.push_back(std::move(tri));
    }
    return out;
}

namespace {
    struct Diagonal {
        Diagonal() { }

        Diagonal(unsigned int p1, unsigned int p2)
        : v1(p1), v2(p2), removed(false)
        {
        }

        // endpoint indices on the polygon vertex array
        unsigned int v1, v2;
        // has this diagonal been removed?
        bool removed;
    };

    void addDiagonal(const std::vector<int> &triangles, int i, int j, int c, std::vector<Diagonal> &diagonals, std::unordered_set<int> &dset)
    {
        unsigned int p1 = triangles[i], p2 = triangles[j];
        // This is a diagonal if the two points are not consecutive
        if((p1+1)%c != p2) {
            if(p2<p1)
                std::swap(p1, p2);

            // Store only unique diagonals
            int id = p1 * 1024 + p2;
            if(!dset.count(id)) {
                dset.insert(id);
                diagonals.push_back(Diagonal(p1, p2));
            }
        }
    }

    void polySplit(const Points &edges, const std::vector<Diagonal> &diagonals, unsigned int i, unsigned int d, std::vector<bool> &visited, std::vector<Points> &out)
    {
        for(;i<edges.size();++i) {
            out.back().push_back(edges[i]);
            if(visited[i])
                break;
            visited[i] = true;

            while(d<diagonals.size() && diagonals[d].v1 <= i) {
                if(!diagonals[d].removed && diagonals[d].v1 == i) {
                    polySplit(edges, diagonals, diagonals[d].v2, d, visited, out);
                    out.back().push_back(edges[i]);
                }
                ++d;
            }
        }
        out.push_back(Points());
    }

    /**
     * Given three points on a polygon, check if p3 is to the left.
     *
     * For a CCW winding polygon, this can be used to check if
     * the point is reflex or convex.
     * @param p1
     * @param p2
     * @param p3
     * @return <0 if point is to the left
     */
    bool isReflex(const Point &p1, const Point &p2, const Point &p3)
    {
        glm::vec2 v1 = p2-p1;
        glm::vec2 v2 = p3-p2;
        return v1.x*v2.y - v1.y*v2.x < 0;
    }

    /**
     * Get a vertex from a point list, wrapping around.
     *
     * -1 will return the last vertex and points.size() the first.
     * @param points point list
     * @param index point index
     * @return point at index
     */
    Point vertex(const Points &points, int index)
    {
        if(index<0)
            return points[points.size() + index];
        else if(index>=int(points.size()))
            return points[index - points.size()];
        return points[index];
    }

    /**
     * Check if removing the diagonal would make vertex v1 reflex
     */
    bool isDiagonalEssential1(const Points &polygon, const std::vector<Diagonal> &diagonals, int i)
    {
        const Diagonal &di = diagonals[i];
        // first, find the previous edge
        Point e1, e2;
        int dp=i;
        while(dp>=0) {
            --dp;
            if(dp<0 || diagonals[dp].v1 != di.v1) {
                e1 = vertex(polygon, di.v1-1);
                e2 = vertex(polygon, di.v1);
                break;
            }
            if(!diagonals[dp].removed) {
                e1 = polygon[diagonals[dp].v2];
                e2 = polygon[diagonals[dp].v1];
                break;
            }
        }

        // then, find the first point of the next edge
        Point e3;
        unsigned int dn=i;
        while(dn <= diagonals.size()) {
            ++dn;
            if(dn==diagonals.size() || diagonals[dn].v1 != di.v1) {
                e3 = vertex(polygon, di.v1+1);
                break;
            }
            if(!diagonals[dn].removed) {
                e3 = polygon[diagonals[dn].v2];
                break;
            }
        }

        return isReflex(e1, e2, e3);
    }

    /**
     * Check if removing the diagonal would make vertex v2 reflex
     */
    bool isDiagonalEssential2(const Points &polygon, const std::vector<Diagonal> &diagonals, int i)
    {
        const Diagonal &di = diagonals[i];
        // first, find the previous edge
        Point e1, e2;
        unsigned int dp=i;
        while(dp <= diagonals.size()) {
            ++dp;
            if(dp==diagonals.size() || (diagonals[dp].v1!=di.v1 && diagonals[dp].v2!=di.v2)) {
                e1 = vertex(polygon, di.v2-1);
                e2 = vertex(polygon, di.v2);
                break;
            }
            if(!diagonals[dp].removed && diagonals[dp].v1!=di.v1) {
                e1 = polygon[diagonals[dp].v1];
                e2 = polygon[diagonals[dp].v2];
                break;
            }
        }

        // next, find the next edge
        Point e3;
        int dn=i;
        while(dn>=0) {
            --dn;
            if(dn<0 || (diagonals[dn].v1!=di.v1 && diagonals[dn].v2!=di.v2)) {
                e3 = vertex(polygon, di.v2+1);
                break;
            }
            if(!diagonals[dn].removed && diagonals[dn].v1!=di.v1) {
                e3 = polygon[diagonals[dn].v1];
                break;
            }
        }
 
        return isReflex(e1, e2, e3);
    }

}

std::vector<Points> fastPartition(const Points &polygon)
{
    // Hertel-Mehlhorn partitioning

    // Triangulate polygon
    std::vector<int> triangles = triProcess(polygon);

    // Get a list of unique diagonals
    // Every edge not part of the outline is a diagonal.
    // Outline edges are all consecutive.
    std::vector<Diagonal> diagonals;
    std::unordered_set<int> dset;
    const int c = polygon.size();
    for(unsigned int i=0;i<triangles.size();i+=3) {
        addDiagonal(triangles, i,   i+1, c, diagonals, dset);
        addDiagonal(triangles, i+1, i+2, c, diagonals, dset);
        addDiagonal(triangles, i+2, i,   c, diagonals, dset);
    }

    // Order diagonal list: v1 ascending, v2 descending
    std::sort(diagonals.begin(), diagonals.end(), [](const Diagonal &i, const Diagonal &j)->bool {
        if(i.v1 == j.v1)
            return i.v2>=j.v2;
        return i.v1<j.v1;
    });

    // Check which vertices are reflex.
	std::vector<bool> reflex;
    reflex.push_back(isReflex(polygon[polygon.size()-1], polygon[0], polygon[1]));
    for(unsigned int i=1;i<polygon.size()-1;++i)
        reflex.push_back(isReflex(polygon[i-1], polygon[i], polygon[i+1]));
    reflex.push_back(isReflex(polygon[polygon.size()-2], polygon[polygon.size()-1], polygon[0]));

    // Remove inessential diagonals.
    for(unsigned int i=0;i<diagonals.size();++i) {
        Diagonal &di = diagonals[i];
        // See if removing this diagonal would create a reflex vertex
        di.removed =
            !(reflex[di.v1] && isDiagonalEssential1(polygon, diagonals, i)) &&
            !(reflex[di.v2] && isDiagonalEssential2(polygon, diagonals, i));
    }

	// Split into polygons
    std::vector<Points> result(1);
	std::vector<bool> visited(polygon.size());
    polySplit(polygon, diagonals, 0, 0, visited, result);

    result.pop_back(); // remove the last empty polygon
    return result;
}

bool lineIntersection(const Point &p0, const Point &p1, const Point &p2, const Point &p3, Point &p)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1.x - p0.x;     s1_y = p1.y - p0.y;
    s2_x = p3.x - p2.x;     s2_y = p3.y - p2.y;

    float s, t;
    s = (-s1_y * (p0.x - p2.x) + s1_x * (p0.y - p2.y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0.y - p2.y) - s2_y * (p0.x - p2.x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        p = Point(
            p0.x + (t * s1_x),
            p0.y + (t * s1_y)
            );
        return true;
    }

    return false;
#if 0
    float d = (a1.x-a2.x)*(b1.y-b2.y) - (a1.y-a2.y)*(b1.x-b2.x);

    cerr << "ISECT [" << a1.x  << ", " << a1.y << "]-[" << a2.x << "," << a2.y << "] & [" << b1.x << "," << b1.y << "],[" << b2.x << "," << b2.y << "]" << endl;
 
    // parellel lines
    if (d == 0) {
        if(glm::abs(a1.y - b1.y) < EPSILON) {
            float x0 = std::min(a1.x, a2.x);
            float x1 = std::max(a1.x, a2.x);
            float x2 = std::min(b1.x, b2.x);
            float x3 = std::max(b1.x, b2.x);
            if(x0 < x3 && x1 > x2) {
                p = Point(x0+(x1-x2)/2.0, a1.y);
                return true;
            }
        }
        return false;
    }

    p = Point(
        ((b1.x-b2.x)*(a1.x*a2.y-a1.y*a2.x)-(a1.x-a2.x)*(b1.x*b2.y-b1.y*b2.x))/d,
        ((b1.y-b2.y)*(a1.x*a2.y-a1.y*a2.x)-(a1.y-a2.y)*(b1.x*b2.y-b1.y*b2.x))/d
        );

    if(p.x < std::min(a1.x, a2.x) || p.x > std::max(a1.x, a2.x))
        return false;
    if(p.x < std::min(b1.x, b2.x) || p.x > std::max(b1.x, b2.x))
        return false;
    return true;
#endif
}

}
}
