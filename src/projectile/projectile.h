#ifndef LUOLA_PROJECTILE_H
#define LUOLA_PROJECTILE_H

#include "../physics.h"

class ProjectileDef;

class Projectile
{
public:
    Projectile() = default;

    /**
     * Create a new projectile.
     *
     * @param def projectile definition
     * @param pos initial position
     * @param vel initial velocity
     */
    Projectile(const ProjectileDef *def, const glm::vec2 &pos, const glm::vec2 &vel);

    /**
     * Get the physics object of the projectile
     * @return physics
     */
    Physical &physics() { return m_physics; }
    const Physical &physics() const { return m_physics; }

    /**
     * Get the projectile definition object
     * @return projectile definition
     */
    const ProjectileDef *def() const { return m_def; }

private:
    Physical m_physics;
    const ProjectileDef *m_def;

};

#endif

