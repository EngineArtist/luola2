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
#include <glm/gtc/matrix_transform.hpp>

#include "projectiledef.h"
#include "projectile.h"
#include "../res/model.h"

Projectile::Projectile(const ProjectileDef *def, const glm::vec2 &pos, const glm::vec2 &vel)
    : m_physics(def->mass(), def->radius(), pos, vel), m_def(def)
{
}

void Projectile::draw(const glm::mat4 &transform) const
{
    glm::mat4 m = glm::scale(glm::translate(transform, glm::vec3(m_physics.position(), 0.0f)), glm::vec3(m_def->radius()));

    Projectiles::getModel()->render(m, m_def->mesh().first, m_def->mesh().second);
}
