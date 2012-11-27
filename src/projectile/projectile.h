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

    /**
     * Draw the projectile
     * 
     * @param transform
     */
    void draw(const glm::mat4 &transform) const;

private:
    Physical m_physics;
    const ProjectileDef *m_def;

};

#endif

