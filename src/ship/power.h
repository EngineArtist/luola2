//
// This file is part of Luola2.
// Copyright (C) 2012 Calle Laakkonen
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
#ifndef LUOLA_SHIP_POWER_H
#define LUOLA_SHIP_POWER_H

#include <string>
#include <unordered_map>

using std::string;

namespace conftree { class Node; }
namespace fs { class DataFile; }

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
     * @return amount of energy generated in a single physics tick
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
    static void loadAll(fs::DataFile &df, const string &filename);

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

