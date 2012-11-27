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
