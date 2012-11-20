#include "../util/conftree.h"
#include "../ship/ship.h"
#include "projectiledef.h"

/**
 * A simple bullet that just explodes and does damage.
 */
class SimpleBullet : public ProjectileDef
{
public:
    SimpleBullet(const conftree::Node &node)
    : ProjectileDef(node)
    {
    }

private:
};

namespace {
    ProjectileFactory<SimpleBullet> FACTORY("simplebullet");
}
