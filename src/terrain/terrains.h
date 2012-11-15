#ifndef LUOLA_TERRAINS_H
#define LUOLA_TERRAINS_H

#include "../util/optional.h"
#include "terrain.h"

namespace terrain {

/**
 * Properties of a Zone.
 *
 * Any given given point in the world has a set of properties associated
 * with it, such as the gravity vector and air density. For a point not
 * in any explicitly defined zone, they will come directly from the root zone.
 * A point inside one or more zone has the properties of those zones. Some
 * properties (such as air density) overwrite each other (the latest zone wins)
 * while some are cumulative (such as force vectors.)
 */
struct ZoneProps {
    /// The force vector in the zone. This is the gravity vector in the root zone
    glm::vec2 force;

    /// The medium density (0 for space)
    float density;
};

/**
 * A specialized flythrough terrain type.
 *
 * A zone is typically immutable and no collision detection is done for it.
 * It is used to set the properties of the space.
 */
class Zone : public Terrain {
public:
    /**
     * Apply this zone to the given ZoneInfo object.
     *
     * @param z the zone info accumulator
     */
    void apply(ZoneProps &z) const;

private:
    glm::vec2 m_force;
    Optional<float> m_density;
};

/**
 * Normal solid terrain.
 *
 * Solid terrain can be either indestructible or destructible.
 */
class Solid : public Terrain {
public:
    Solid(const std::vector<ConvexPolygon> polygons) : Terrain(polygons) { }
};

}

#endif

