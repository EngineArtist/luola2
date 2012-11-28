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
#include <stdexcept>

#include "../util/conftree.h"
#include "../physics.h"
#include "exception.h"
#include "power.h"

namespace {
    PowerPlants *POWERPLANTS;
}

PowerPlant::PowerPlant(const conftree::Node &node)
{
    m_energy = node.at("energy").floatValue() * Physical::TIMESTEP;
}

PowerPlants &PowerPlants::getInstance()
{
    if(!POWERPLANTS)
        POWERPLANTS = new PowerPlants();
    return *POWERPLANTS;
}

void PowerPlants::loadAll(fs::DataFile &df, const string &filename)
{
    conftree::Node node = conftree::parseYAML(df, filename);

    PowerPlants &pp = getInstance();
    for(const string &key : node.itemSet())
        pp.m_pplants[key] = new PowerPlant(node.at(key));
}

const PowerPlant *PowerPlants::get(const string &name)
{
    const PowerPlants &pp = getInstance();

    try {
        return pp.m_pplants.at(name);
    } catch(const std::out_of_range &ex) {
        throw ShipDefException("power plant \"" + name + "\" not defined!");
    }
}
