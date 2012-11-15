#ifndef NDEBUG
#include <iostream>
using std::cerr;
using std::endl;
#endif

#include <yaml-cpp/yaml.h>

#include "../fs/datafile.h"
#include "../res/loader.h"
#include "../res/model.h"

#include "exception.h"
#include "shipdef.h"

namespace {
    string optValue(const YAML::Node &node, const char *key, const string &def)
    {
        const YAML::Node *n = node.FindValue(key);
        if(n)
            return n->to<string>();
        return def;
    }
}

ShipDef::ShipDef(ResourceLoader &resloader, const YAML::Node &doc)
{
    doc["shortname"] >> m_shortname;
    m_fullname = optValue(doc, "fullname", m_shortname);

    doc["mass"] >> m_mass;
    doc["radius"] >> m_radius;

    m_turnrate = glm::radians(doc["turningrate"].to<float>());

    string model = doc["model"].to<string>();
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
    std::unique_ptr<YAML::Node> def(new YAML::Node());
    {
        DataStream ds(df, "ship.yaml");
        if(ds->isError())
            throw ShipDefException(shipname + ": error reading ship.yaml!");

        YAML::Parser parser(ds);
        if(!parser.GetNextDocument(*def))
            throw ShipDefException(shipname + ": ship.yaml is not a YAML file!");

        if(def->Type() != YAML::NodeType::Map)
            throw ShipDefException(shipname + ": ship.yaml should contain a Map!");
    }

    ShipDef *shipdef = new ShipDef(rl, *def);

    shipdefs.m_shipdefs[shipname] = shipdef;
}

const ShipDef &ShipDefs::get(const string &name)
{
    const ShipDefs &sd = getInstance();
    try {
        return *sd.m_shipdefs.at(name);
    } catch(const std::out_of_range &ex) {
        throw ShipDefException("definition for ship \"" + name + "\" not loaded!");
    }
}
