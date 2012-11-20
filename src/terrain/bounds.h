#ifndef LUOLA_TERRAIN_BOUNDS_H
#define LUOLA_TERRAIN_BOUNDS_H

#include "common.h"

namespace terrain {

/**
 * A bounding rectangle
 */
class BRect {
public:
    /**
     * Construct a blank bounding rectangle
     */
    BRect() = default;

    /**
     * Construct a bounding rectangle from corner coordinates
     *
     * @param btmleft bottom left corner
     * @param topright top right corner
     */
    BRect(const Point &btmleft, const Point &topright);

    /**
     * Construct a bounding rectangle that encompasses the given list of points.
     *
     * @param points list of points
     */
    BRect(const std::vector<Point> &points);
    
    /**
     * Get the bottom left coordinate of the rectangle.
     * @return bottom left point
     */
    const Point &btmleft() const { return m_btmleft; }
    
    /**
     * Get the top right coordinate of the rectangle.
     * @return top right point
     */
    const Point &topright() const { return m_topright; }
    
    /**
     * Leftmost X coordinate
     * @return x0
     */
    float left() const { return m_btmleft.x; }
    
    /**
     * Rightmost X coordinate
     * @return x1
     */
    float right() const { return m_topright.x; }
    
    /**
     * Bottom Y coordinate
     * @return y0
     */
    float bottom() const { return m_btmleft.y; }
    
    /**
     * Topmost X coordinate
     * @return y1
     */
    float top() const { return m_topright.y; }
    
    /**
     * Get the width of the bounding rectangle
     *
     * @return width
     */
    float width() const { return m_topright.x - m_btmleft.x; }
    
    /**
     * Get the height of the bounding rectangle
     *
     * @return height
     */
    float height() const { return m_topright.y - m_btmleft.y; }
    
    /**
     * Check if this bounding box overlaps another one.
     *
     * @param other the other box to check
     * @return true if boxes overlap
     */
    bool overlaps(const BRect &other) const;

private:
    Point m_btmleft, m_topright;
};

}

#endif

