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
#include <cmath>

#include "../util/conftree.h"
#include "../world.h"
#include "weapon.h"
#include "../projectile/projectiledef.h"

/**
 * A generic launcher for projectiles.
 */
class Launcher : public Weapon
{
public:
    Launcher(const conftree::Node &node)
    : Weapon(node)
    {
        // launch force
        m_force = node.at("force").floatValue();
    }

    void fire(const ProjectileDef *projectile, Ship &ship, World &world) const
    {
        assert(projectile);

        Projectile p = projectile->make(ship.physics().position(), ship.physics().velocity());
        p.physics().addImpulse(glm::vec2(
            cos(ship.angle()) * m_force,
            sin(ship.angle()) * m_force
            ));

        world.addProjectile(p);
    }

private:
    float m_force;
};

namespace {
    WeaponFactory<Launcher> FACTORY("launcher");
}
