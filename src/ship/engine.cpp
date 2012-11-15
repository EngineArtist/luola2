#include <yaml-cpp/yaml.h>

#include "../fs/datafile.h"
#include "../physics.h"
#include "exception.h"
#include "engine.h"

namespace {
    Engines *ENGINES;
}

Engine::Engine(const YAML::Node &node)
{
    m_thrust = node["thrust"].to<float>() * Physical::TIMESTEP;
}

Engines &Engines::getInstance()
{
    if(!ENGINES)
        ENGINES = new Engines();
    return *ENGINES;
}

void Engines::loadAll(DataFile &df, const string &filename)
{
    Engines &e = getInstance();

    DataStream ds(df, filename);
    if(ds->isError())
        throw ShipDefException("error reading engine definition file " + filename);

    std::unique_ptr<YAML::Node> defs(new YAML::Node());
    YAML::Parser parser(ds);
    if(!parser.GetNextDocument(*defs))
        throw ShipDefException(filename + ": error reading YAML file!");

    if(defs->Type() != YAML::NodeType::Map)
        throw ShipDefException(filename + ": not a Map!");

    for(YAML::Iterator it=defs->begin();it!=defs->end();++it) {
        string name = it.first().to<string>();
        e.m_engines[name] = new Engine(it.second());
    }
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
