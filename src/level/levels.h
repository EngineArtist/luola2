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
#ifndef LUOLA_LEVELS_H
#define LUOLA_LEVELS_H

#include <string>
#include <unordered_map>

using std::string;

class World;

namespace conftree { class Node; }

namespace level {

class Level {
public:
    Level() = default;
    Level(const string &name, const conftree::Node &meta);

    /**
     * Get the internal name of the levels
     *
     * This is the name that identifies the level inside the
     * system. It is the name of the level file without the extension.
     *
     * @return level name
     */
    const string &name() const { return m_name; }

    /**
     * Get the user visible name of the levels
     *
     * The title is name shown to players.
     *
     * @return level title
     */
    const string &title() const { return m_title; }
    
    /**
     * Get the name of the level data file
     *
     * @return data file name
     */
    string datafile() const;
 
    /**
     * Load the level
     *
     * @param world game world
     */
    void load(World &world) const;
 
private:
    string m_name;
    string m_title;
};

/**
 * List of available levels
 */
class LevelRegistry {
public:
    /**
     * Initialize the level registry and scan data directories for levels.
     */
    static void init();

    /**
     * Get the level with the given name
     *
     * @return level
     */
    static const Level &get(const string &name);

    static void add(const string &filename);

private:
    /**
     * Load level metadata
     * @param filename name of the level file
     */
    void addLevel(const string &filename);

    std::unordered_map<string, Level> m_levels;
};

}

#endif
