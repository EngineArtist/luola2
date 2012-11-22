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
