#include <yaml-cpp/yaml.h>

#include "../fs/datafile.h"
#include "../physics.h"
#include "exception.h"
#include "power.h"

namespace {
    PowerPlants *POWERPLANTS;
}

PowerPlant::PowerPlant(const YAML::Node &node)
{
    m_energy = node["energy"].to<float>() * Physical::TIMESTEP;
}

PowerPlants &PowerPlants::getInstance()
{
    if(!POWERPLANTS)
        POWERPLANTS = new PowerPlants();
    return *POWERPLANTS;
}

void PowerPlants::loadAll(DataFile &df, const string &filename)
{
    PowerPlants &pp = getInstance();

    DataStream ds(df, filename);
    if(ds->isError())
        throw ShipDefException("error reading power plant definition file " + filename);

    std::unique_ptr<YAML::Node> defs(new YAML::Node());
    YAML::Parser parser(ds);
    if(!parser.GetNextDocument(*defs))
        throw ShipDefException(filename + ": error reading YAML file!");

    if(defs->Type() != YAML::NodeType::Map)
        throw ShipDefException(filename + ": not a Map!");

    for(YAML::Iterator it=defs->begin();it!=defs->end();++it) {
        string name = it.first().to<string>();
        pp.m_pplants[name] = new PowerPlant(it.second());
    }
}

const PowerPlant &PowerPlants::get(const string &name)
{
    const PowerPlants &pp = getInstance();

    try {
        return *pp.m_pplants.at(name);
    } catch(const std::out_of_range &ex) {
        throw ShipDefException("power plant \"" + name + "\" not defined!");
    }
}
