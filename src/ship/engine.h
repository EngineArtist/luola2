#ifndef LUOLA_SHIP_ENGINE_H
#define LUOLA_SHIP_ENGINE_H

#include <string>
#include <unordered_map>

using std::string;

namespace YAML { class Node; }
class DataFile;

/**
 * Ship engine definition
 */
class Engine {
public:
    Engine(const YAML::Node &node);

    /**
     * Get the thrusting power
     *
     * The thrust value is premultiplied with the physics timestep.
     * @return engine thrust in force/s
     */
    float thrust() const { return m_thrust; }

private:
    float m_thrust;
};

class Engines {
public:
    Engines() {}
    Engines(const Engines&) = delete;

    /**
     * Load all engine definitions from the data file
     * 
     * @param df the datafile
     * @param filename engine definition file name
     */
    static void loadAll(DataFile &df, const string &filename);

    /**
     * Get the named engine definition
     *
     * If not found, EngineDefException will be thrown
     * 
     * @param name engine name
     * @return shared engine instance
     */
    static const Engine &get(const string &name);

private:
    static Engines &getInstance();

    std::unordered_map<string, Engine*> m_engines;
};

#endif
