#include "grid.h"

namespace index {

struct CellElement {
    BRect rect;
    Element el;
};

struct GridCell {
    std::vector<CellElement> elements;
};

Grid::Grid(const BRect &bounds, float size)
 : m_size(size), m_bounds(bounds)
{
    m_width = int(ceil(bounds.width() / size));
    m_height = int(ceil(bounds.height() / size));
    m_nodes = new GridCell[m_width * m_height];
}

Grid::~Grid()
{
    delete[] m_nodes;
}

void Grid::addElement(const BRect &rect, const Element &el)
{
    const CellElement ce = {rect, el};
    forEachCell(rect, [&ce](GridCell &gc) { gc.elements.push_back(ce); });
}

void Grid::addUniqueElement(const BRect &rect, const Element &el)
{
    const CellElement ce = {rect, el};
    forEachCell(rect, [&](GridCell &gc) {
        for(const CellElement &e : gc.elements) {
            if(e.el == el)
                return;
        }
        gc.elements.push_back(ce);
    });
}

void Grid::removeElement(const BRect &rect, const Element &el)
{
    forEachCell(rect, [&](GridCell &gc) {
        unsigned int i=0;
        while(i<gc.elements.size()) {
            if(gc.elements[i].el == el) {
                if(i+1 < gc.elements.size())
                    gc.elements[i] = gc.elements.back();
                gc.elements.pop_back();
            } else {
                ++i;
            }
        }
    });
}

void Grid::getElements(const BRect &area, bool unique, std::vector<Element> &elements)
{
    forEachCell(area, [&](GridCell &gc) {
        for(const CellElement &ce : gc.elements) {
            if(ce.rect.overlaps(area)) {
                elements.push_back(ce.el);
            }
        }
    });
}

void Grid::forEachCell(const BRect &rect, const std::function<void(GridCell&)> &action)
{
    float y = std::max(m_bounds.bottom(), rect.bottom()) - m_bounds.bottom();
    const float y1 = std::min(m_bounds.top(), rect.top()) - m_bounds.bottom();
    const float x0 = std::max(m_bounds.left(), rect.left()) - m_bounds.left();
    const float x1 = std::min(m_bounds.right(), rect.right()) - m_bounds.left();

    int iy = int(y/m_bounds.height() * m_height);
    while(y<=y1) {
        float x = x0;
        int ix = int(x/m_bounds.width() * m_width);
        while(x<=x1) {
            action(m_nodes[iy * m_width + ix]);
            ++ix;
            x += m_size;
        }
        ++iy;
        y += m_size;
    }
}

}
