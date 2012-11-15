#include "terrains.h"

namespace terrain {

void Zone::apply(ZoneProps &z) const
{
    z.force += m_force;
    m_density.assign(z.density);
}

}