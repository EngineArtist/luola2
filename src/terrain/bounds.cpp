#include "bounds.h"

namespace terrain {

BRect::BRect(const glm::vec2 &btmleft, const glm::vec2 &topright)
: m_btmleft(btmleft), m_topright(topright)
{
    assert(width() >= 0);
    assert(height() >= 0);
}

BRect::BRect(const std::vector<glm::vec2> &points)
{
    float x0=points[0].x, y0=points[0].y;
    float x1=x0, y1=y0;
    for(const glm::vec2 &p : points) {
        if(p.x < x0)
            x0 = p.x;
        if(p.x > x1)
            x1 = p.x;
        if(p.y < y0)
            y0 = p.y;
        if(p.y > y1)
            y1 = p.y;
    }
    m_btmleft = glm::vec2(x0, y0);
    m_topright = glm::vec2(x1, y1);
}

bool BRect::overlaps(const BRect &other) const
{
    return left() < other.right() && right() > other.left() &&
    top() > other.bottom() && bottom() < other.top();
}

}
