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
    glm::mat4 m = glm::translate(transform, glm::vec3(m_physics.position(), 0.0f));

    m_def->model()->render(m);
}
