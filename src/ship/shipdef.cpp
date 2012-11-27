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
#ifndef NDEBUG
#include <iostream>
using std::cerr;
using std::endl;
#endif

#include <stdexcept>

#include "../fs/datafile.h"
#include "../util/conftree.h"
#include "../res/loader.h"
#include "../res/model.h"

#include "exception.h"
#include "shipdef.h"

ShipDef::ShipDef(resource::Loader &resloader, const conftree::Node &doc)
{
    m_shortname = doc.at("shortname").value();
    m_fullname = doc.opt("fullname").value(m_shortname);

    m_mass = doc.at("mass").floatValue();
    m_radius = doc.at("radius").floatValue();

    m_turnrate = glm::radians(doc.at("turningrate").floatValue());

    string model = doc.at("model").value();
    m_model = dynamic_cast<resource::Model*>(resloader.load(model));
    if(!m_model)
        throw ShipDefException("unable to load model " + model);
}


namespace {
    ShipDefs *SHIPDEFS;
}

ShipDefs &ShipDefs::getInstance()
{
    if(!SHIPDEFS)
        SHIPDEFS = new ShipDefs();

    return *SHIPDEFS;
}

void ShipDefs::loadAll(fs::DataFile &datafile, const string &filename)
{
    conftree::Node ships = conftree::parseYAML(datafile, filename);

#ifndef NDEBUG
    cerr << "Loading " << ships.items() << " ship definitions.\n";
#endif
    for(unsigned int i=0;i<ships.items();++i) {
        load(ships.at(i).value());
    }
}

void ShipDefs::load(const string &shipname)
{
    ShipDefs &shipdefs = getInstance();

    // Make sure the ship hasn't been loaded already
    if(shipdefs.m_shipdefs.count(shipname)) {
#ifndef NDEBUG
        cerr << "Warning! Tried to reload ship definition \"" << shipname << "\"" << endl;
#endif
        return;
    }

    // Open ship data file
    fs::DataFile df(shipname + ".ship");
    if(df.isError())
        throw ShipDefException(df.errorString());

    // Load ship resources
    resource::Loader rl(df, "resources.yaml");

    // Load ship definition file
    conftree::Node def = conftree::parseYAML(df, "ship.yaml");
    ShipDef *shipdef = new ShipDef(rl, def);

    shipdefs.m_shipdefs[shipname] = shipdef;
}

const ShipDef *ShipDefs::get(const string &name)
{
    const ShipDefs &sd = getInstance();
    try {
        return sd.m_shipdefs.at(name);
    } catch(const std::out_of_range &ex) {
        throw ShipDefException("definition for ship \"" + name + "\" not loaded!");
    }
}
