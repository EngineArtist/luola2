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

ShipDef::ShipDef(ResourceLoader &resloader, const conftree::Node &doc)
{
    m_shortname = doc.at("shortname").value();
    m_fullname = doc.opt("fullname").value(m_shortname);

    m_mass = doc.at("mass").floatValue();
    m_radius = doc.at("radius").floatValue();

    m_turnrate = glm::radians(doc.at("turningrate").floatValue());

    string model = doc.at("model").value();
    m_model = dynamic_cast<ModelResource*>(resloader.load(model));
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

void ShipDefs::loadAll(DataFile &datafile, const string &filename)
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
    DataFile df(shipname + ".ship");
    if(df.isError())
        throw ShipDefException(df.errorString());

    // Load ship resources
    ResourceLoader rl(df, "resources.yaml");

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
