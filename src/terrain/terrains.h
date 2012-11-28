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
    Zone(const std::vector<ConvexPolygon> polygons) : Terrain(polygons) { }
 
    /**
     * Apply this zone to the given ZoneInfo object.
     *
     * @param z the zone info accumulator
     */
    void apply(ZoneProps &z) const;

    /**
     * Set the zone vector
     */
    void setZoneForce(const glm::vec2 &force) { m_force = force; }

    /**
     * Set zone air density
     */
    void setZoneDensity(float density) { m_density = density; }
 
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

