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
#include <stdexcept>

#include "../util/conftree.h"
#include "../ship/exception.h"
#include "equipment.h"

Equipment::Equipment(const conftree::Node &node)
{
}

Equipment::~Equipment()
{
}

EquipmentFactoryBase::~EquipmentFactoryBase()
{
}

void Equipments::loadAll(fs::DataFile &df, const string &filename)
{
    conftree::Node node = conftree::parseYAML(df, filename);

    Equipments &e = getInstance();
    for(const string &key : node.itemSet()) {
        const conftree::Node &n = node.at(key);
        e.m_equipment[key] = e.m_factories.at(n.at("codebase").value())->make(n);
    }
}

const Equipment *Equipments::get(const string &name)
{
    const Equipments &e = getInstance();
    try {
        return e.m_equipment.at(name);
    } catch(const std::out_of_range &ex) {
        throw ShipDefException("equipment \"" + name + "\" not defined!");
    }
}

void Equipments::registerFactory(const string &name, EquipmentFactoryBase *factory)
{
    m_factories[name] = factory;
}

namespace {
    Equipments *EQUIPMENTS;
}

Equipments &Equipments::getInstance()
{
    if(!EQUIPMENTS)
        EQUIPMENTS = new Equipments();
    return *EQUIPMENTS;
}
