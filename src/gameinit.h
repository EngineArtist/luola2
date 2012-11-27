//
// This file is part of Luola2.
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
#ifndef LUOLA_GAMEINIT_H
#define LUOLA_GAMEINIT_H

#include <array>
#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "util/optional.h"
#include "ship/ship.h"

using std::string;

class World;

namespace gameinit {

struct WeaponConf {
    // Name of the weapon
    string weapon;

    // Name of the projectile to use as ammo.
    // Note. Not all weapons have selectable ammo!
    // For those, an empty string should be used.
    string ammo;
};

typedef std::array<WeaponConf, MAX_WEAPONS> WeaponConfs;

/**
 * Configurable ship attributes
 */
struct ShipConf {
    int player;
    int team;
    string hull;
    string power;
    string engine;
    std::vector<string> equipment;
    WeaponConfs weapons;
    Optional<glm::vec2> position;
};

/**
 * Hotseat game initialization
 *
 */
class Hotseat {
public:
    /**
     * Maximum number of players in a hotseat game
     */
    static const int MAX_PLAYERS = 4;

    /**
     * Load game initialization data from an external file.
     *
     * This is typically used while testing the game to quickly launch
     * specific configurations. It can also be used by an external launcher.
     *
     * @param filename path to the launch configuration file
     */
    static Hotseat loadFromFile(const string &filename);

    /**
     * Prepare the World for the game session.
     *
     * This loads the level, adds players, positions ships and such.
     *
     * @param world the World to initialize
     */
    void initialize(World &world) const;

    /**
     * Add a ship
     *
     * @param shipconf ship configuration
     */
    void addShip(const ShipConf &shipconf);

private:
    std::vector<ShipConf> m_ships;
};

class GameInitError : public std::exception {
public:
    GameInitError(const string &error);
    ~GameInitError() throw();

    const char *what() const throw();
    friend std::ostream& operator<<(std::ostream&, const GameInitError&);

private:
    string m_error;
};

}

#endif

