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
#ifndef LUOLA_WORLD_H
#define LUOLA_WORLD_H

#include <vector>

#include "terrain/terrains.h"
#include "ship/ship.h"
#include "projectile/projectile.h"

/**
 * Game world state
 */
class World {
    friend class Renderer;
public:

    // Simulate one timestep
    void step();

    //// Querying

    /**
     * Get the ship owned by the given player.
     *
     * This may return null if the player isn't in play
     * or the ship has been destroyed.
     *
     * @return player ship or nullptr
     */
    Ship *getPlayerShip(int player);

    /**
     * Get the zone properties at the given point.
     *
     * @param p point in level space
     * @returns zone properties
     */
    terrain::ZoneProps zoneAt(const terrain::Point &p) const;

    /**
     * Check for terrain collisions with a moving circle.
     *
     * @param circle center
     * @param circle radius
     * @param v circle displacement in this timestep
     * @param cp circle's center point at collision
     * @param normal normal of the colliding edge
     * @return true if collision detected
     */
    bool checkTerrainCollision(const terrain::Point &p, float r, const glm::vec2 &v, terrain::Point &cp, glm::vec2 &normal) const;

    //// Manipulation

    /**
     * Add a new ship to the world.
     *
     * @param player the player to whom the ship belongs to
     * @param ship the ship to add
     */
    void addShip(const Ship &ship);

    /**
     * Add a new projectile to the world.
     *
     * @param projectile the projectile
     */
    void addProjectile(const Projectile &projectile);

    /**
     * Set root zone properties
     *
     * @param props zone properties
     */
    void setRootZone(const terrain::ZoneProps &props) { m_rootzone = props; }

    /**
     * Set world boundaries
     *
     * @param bounds boundaries
     */
    void setBounds(const terrain::BRect &bounds) { m_bounds = bounds; }

    /**
     * Get world boundaries
     *
     * @return boundaries
     */
    const terrain::BRect &bounds() const { return m_bounds; }

    /**
     * Add a new zone to the world.
     *
     * The world object takes ownership of the pointer.
     *
     * @param zone the zone to add
     */ 
    void addZone(terrain::Zone *zone);

    /**
     * Add destructible terrain to the world.
     *
     * The world class takes ownership of the pointer.
     *
     * @param terrain the terrain block to add.
     */
    void addSolid(terrain::Solid *terrain);

    /**
     * Add indestructible solid terrain to the world.
     *
     * The world class takes ownership of the pointer
     *
     * @param terrain the terrain block to add
     */
    void addStaticSolid(terrain::Solid *terrain);

    /**
     * Make a hole in destructible terrain
     *
     * @param hole hole shape
     */
    void makeHole(const terrain::ConvexPolygon &hole);

private:
    // Game objects
    std::vector<Ship> m_ships;
    std::vector<Projectile> m_projectiles;

    // Root zone properties
    terrain::ZoneProps m_rootzone;
    terrain::BRect m_bounds;

    // Flythrough zones
    std::vector<terrain::Zone*> m_zones;

    // Indestructible terrain
    std::vector<terrain::Solid*> m_static_terrain;

    // Destructible terrain
    std::vector<terrain::Solid*> m_dyn_terrain;
};

#endif
