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
