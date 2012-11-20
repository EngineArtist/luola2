#include "projectiledef.h"
#include "projectile.h"

Projectile::Projectile(const ProjectileDef *def, const glm::vec2 &pos, const glm::vec2 &vel)
    : m_physics(def->mass(), def->radius(), pos, vel), m_def(def)
{
}

