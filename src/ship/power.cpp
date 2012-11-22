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
