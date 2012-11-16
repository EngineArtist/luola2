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

void Engines::loadAll(DataFile &df, const string &filename)
{
    conftree::Node node = conftree::parseYAML(df, filename);

    Engines &e = getInstance();
    for(const string &key : node.itemSet())
        e.m_engines[key] = new Engine(node.at(key));
}

const Engine &Engines::get(const string &name)
{
    const Engines &e = getInstance();

    try {
        return *e.m_engines.at(name);
    } catch(const std::out_of_range &ex) {
        throw ShipDefException("engine \"" + name + "\" not defined!");
    }
}
