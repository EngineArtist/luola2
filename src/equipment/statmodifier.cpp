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
#include "../util/conftree.h"
#include "../ship/ship.h"
#include "equipment.h"

/**
 * A simple equipment that modifies the ship's stats, but has no
 * active functionality.
 *
 * The following ship properties can be changed:
 * 
 * - battery: (+/-) add or remove battery capacity
 * - chargarate: set battery charge rate (units per second)
 */
class StatModifier : public Equipment
{
public:
    StatModifier(const conftree::Node &node)
    : Equipment(node)
    {
        m_battery = node.opt("battery").floatValue();
        m_chargerate = node.opt("chargerate").floatValue(99.0f);
    }

    void applyModification(Ship &ship) const
    {
        if(m_battery != 0.0f) {
            ship.addBattery(m_battery, m_chargerate);
        }
    }

private:
    float m_battery;
    float m_chargerate;
};

namespace {
    EquipmentFactory<StatModifier> FACTORY("statmodifier");
}
