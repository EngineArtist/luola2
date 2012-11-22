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
    void addShip(int player, const Ship &ship);

    /**
     * Add a new projectile to the world.
     *
     * @param projectile the projectile
     */
    void addProjectile(const Projectile &projectile);

    // TODO
    Ship *getPlayerShip(int player);

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

    // Flythrough zones
    std::vector<terrain::Zone*> m_zones;

    // Indestructible terrain
    std::vector<terrain::Solid*> m_static_terrain;

    // Destructible terrain
    std::vector<terrain::Solid*> m_dyn_terrain;
};

#endif
