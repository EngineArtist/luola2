#ifndef LUOLA_SHIP_POWER_H
#define LUOLA_SHIP_POWER_H

#include <string>
#include <unordered_map>

using std::string;

namespace conftree { class Node; }
class DataFile;
/**
 * Ship power source definition
 */
class PowerPlant {
public:
    PowerPlant(const conftree::Node &node);

    /**
     * Get the amount of energy generated per second
     *
     * The value is premultiplied with the physics timestep
     * 
     * @return amount of energy generated
     */
    float energy() const { return m_energy; }

private:
    float m_energy;
};

class PowerPlants {
public:
    PowerPlants() = default;
    PowerPlants(const PowerPlants&) = delete;

    /**
     * Load all power plant definitions from the data file
     *
     * @param df the datafile
     * @param filename power plant definition file name
     */
    static void loadAll(DataFile &df, const string &filename);

    /**
     * Get the named power plant definition
     *
     * If not found, EngineDefException will be thrown
     *
     * @param name power plant name
     * @return shared power plant instance
     */
    static const PowerPlant *get(const string &name);
    
private:
    static PowerPlants &getInstance();
    std::unordered_map<string, PowerPlant*> m_pplants;
};

#endif

