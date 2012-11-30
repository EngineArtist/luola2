#ifndef LUOLA_INDEX_GRID_H
#define LUOLA_INDEX_GRID_H

#include <functional>
#include "../terrain/bounds.h"

namespace index {

using terrain::BRect;

struct Element {
public:
    enum { TER_ZONE, TER_STATIC, TER_SOLID, OBJ_SHIP, OBJ_PROJECTILE } type;
    unsigned int index;

    bool operator==(const Element &e) const
    {
        return e.type == type && e.index == index;
    }
};

class GridCell;

/**
 * A grid based spatial index
 */
class Grid {
public:
    /**
    * Construct a rectangular grid made up of square cells.
    *
    * @param bounds grid area
    * @param size grid cell size
    */
    Grid(const BRect &bounds, float size);

    ~Grid();

    /**
    * Add an element to the grid.
    *
    * The element will be added to all cells intersecting
    * with rect.
    *
    * @param rect element bounds
    * @param el the element to add
    */
    void addElement(const BRect &rect, const Element &el);

    /**
    * Like addElement, except extra checks are made to
    * prevent duplicate elements in the same cell.
    *
    * @param rect element bounds
    * @param el the element to add
    */
    void addUniqueElement(const BRect &rect, const Element &el);

    /**
    * Remove the element from all intersection nodes
    *
    * @param rect element bounds
    * @param el the element to remove
    */
    void removeElement(const BRect &rect, const Element &el);

    /**
    * Get all elements intersecting with the given rectangle
    *
    * @param area elements inside or touching this area
    * @param unique ensure no duplicate elements are included
    * @param elements where to
    */
    void getElements(const BRect &area, bool unique, std::vector<Element> &elements);

private:
    void forEachCell(const BRect &rect, const std::function<void(GridCell&)> &action);

    GridCell *m_nodes;
    unsigned int m_width, m_height;
    float m_size;
    BRect m_bounds;
};


}

#endif
