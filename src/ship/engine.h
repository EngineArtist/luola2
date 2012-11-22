#ifndef LUOLA_SHIP_ENGINE_H
#define LUOLA_SHIP_ENGINE_H

#include <string>
#include <unordered_map>

using std::string;

namespace conftree { class Node; }
namespace fs { class DataFile; }

/**
 * Ship engine definition
 */
class Engine {
public:
    Engine(const conftree::Node &node);

    /**
     * Get the thrusting power
     *
     * The thrust value is premultiplied with the physics timestep.
     * @return engine thrust in force/s
     */
    float thrust() const { return m_thrust; }

    /**
     * Get the amount of energy the engine consumes while thrusting.
     *
     * The value is premultiplied with the physics timestep.
     * @return thrust power consumption
     */
    float thrustEnergy() const { return m_thrustenergy; }

    /**
     * Get the amount of energy the engine consumes while idling.
     * 
     * The values is premultiplied with the physics timestep.
     * @return idle power consumption
     */
    float idleEnergy() const { return m_idle; }
 
private:
    float m_thrust;
    float m_thrustenergy, m_idle;
};

class Engines {
public:
    Engines() = default;
    Engines(const Engines&) = delete;

    /**
     * Load all engine definitions from the data file
     * 
     * @param df the datafile
     * @param filename engine definition file name
     */
    static void loadAll(fs::DataFile &df, const string &filename);

    /**
     * Get the named engine definition
     *
     * If not found, EngineDefException will be thrown
     * 
     * @param name engine name
     * @return shared engine instance
     */
    static const Engine *get(const string &name);

private:
    static Engines &getInstance();

    std::unordered_map<string, Engine*> m_engines;
};

#endif
