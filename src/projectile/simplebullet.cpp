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
