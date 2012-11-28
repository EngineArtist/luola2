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
#include "engine.h"

namespace {
    Engines *ENGINES;
}

Engine::Engine(const conftree::Node &node)
{
    m_thrust = node.at("thrust").floatValue() * Physical::TIMESTEP;
    m_thrustenergy = node.at("energy").floatValue() * Physical::TIMESTEP;
    m_idle = node.opt("idle").floatValue() * Physical::TIMESTEP;
}

Engines &Engines::getInstance()
{
    if(!ENGINES)
        ENGINES = new Engines();
    return *ENGINES;
}

void Engines::loadAll(fs::DataFile &df, const string &filename)
{
    conftree::Node node = conftree::parseYAML(df, filename);

    Engines &e = getInstance();
    for(const string &key : node.itemSet())
        e.m_engines[key] = new Engine(node.at(key));
}

const Engine *Engines::get(const string &name)
{
    const Engines &e = getInstance();

    try {
        return e.m_engines.at(name);
    } catch(const std::out_of_range &ex) {
        throw ShipDefException("engine \"" + name + "\" not defined!");
    }
}
