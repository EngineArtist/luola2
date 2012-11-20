#include "../util/conftree.h"
#include "../ship/ship.h"
#include "weapon.h"

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

private:
    float m_force;
};

namespace {
    WeaponFactory<Launcher> FACTORY("launcher");
}
