#include <stdexcept>

#include "../util/conftree.h"
#include "../ship/exception.h"
#include "../physics.h"
#include "weapon.h"

Weapon::Weapon(const conftree::Node &node)
{
    float cooloff = node.opt("cooloff").floatValue(0.0) / Physical::TPS;
    m_cooloff = std::round(cooloff);
}

Weapon::~Weapon()
{
}

WeaponFactoryBase::~WeaponFactoryBase()
{
}

void Weapons::loadAll(DataFile &df, const string &filename)
{
    conftree::Node node = conftree::parseYAML(df, filename);

    Weapons &w = getInstance();
    for(const string &key : node.itemSet()) {
        const conftree::Node &n = node.at(key);
        w.m_weapons[key] = w.m_factories.at(n.at("codebase").value())->make(n);
    }
}

const Weapon *Weapons::get(const string &name)
{
    const Weapons &w = getInstance();
    try {
        return w.m_weapons.at(name);
    } catch(const std::out_of_range &ex) {
        throw ShipDefException("weapon \"" + name + "\" not defined!");
    } 
}

void Weapons::registerFactory(const string &name, WeaponFactoryBase *factory)
{
    m_factories[name] = factory;
}

namespace {
    Weapons *WEAPONS;
}

Weapons &Weapons::getInstance()
{
    if(!WEAPONS)
        WEAPONS = new Weapons();
    return *WEAPONS;
}
