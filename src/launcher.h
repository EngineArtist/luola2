#ifndef LUOLA_LAUNCHER_H
#define LUOLA_LAUNCHER_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "util/optional.h"

using std::string;

class World;

namespace gameinit {

/**
 * Configurable ship attributes
 */
struct ShipConf {
    string hull;
    string power;
    string engine;
    std::vector<string> equipment;
    Optional<glm::vec2> position;
};

/**
 * Hotseat game initialization
 *
 */
class Hotseat {
public:
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
     * @param player the player to whom the ship belongs to [1..4]
     * @param shipconf ship configuration
     */
    void addShip(int player, const ShipConf &shipconf);

private:
    std::vector<std::pair<int, ShipConf>> m_ships;
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

