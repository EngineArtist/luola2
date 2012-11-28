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
#include <ostream>

#include <boost/lexical_cast.hpp>

#include "util/conftree.h"

#include "gameinit.h"
#include "world.h"
#include "ship/ship.h"
#include "terrain/terrains.h"
#include "level/levels.h"

namespace gameinit {

namespace {
    std::vector<string> node2vec(const conftree::Node &node)
    {
        std::vector<string> vec;
        for(unsigned int i=0;i<node.items();++i)
            vec.push_back(node.at(i).value());
        return vec;
    }

    Optional<glm::vec2> node2optvec2(const conftree::Node &node)
    {
        if(node.type() == conftree::Node::BLANK)
            return Optional<glm::vec2>();
        return Optional<glm::vec2>(glm::vec2(
            node.at(0).floatValue(),
            node.at(1).floatValue()));
    }

    WeaponConfs parseWeaponConfs(const conftree::Node &node)
    {
        WeaponConfs confs;
        for(unsigned int i=0;i<confs.size();++i) {
            const conftree::Node wn = node.opt("weapon" + boost::lexical_cast<string>(i+1));
            if(wn.type() != conftree::Node::BLANK) {
                confs[i].weapon = wn.at("weapon").value();
                confs[i].ammo = wn.at("ammo").value();
            }
        }
        return confs;
    }

    ShipConf parseShipConf(int player, const conftree::Node &node)
    {
        return {
            player,
            node.at("team").intValue(),
            node.at("hull").value(),
            node.at("power").value(),
            node.at("engine").value(),
            node2vec(node.opt("equipment")),
            parseWeaponConfs(node),
            node2optvec2(node.opt("position"))
        };
    }
}

Hotseat Hotseat::loadFromFile(const string &filename)
{
    conftree::Node node = conftree::parseYAML(filename);
    Hotseat hs;

    // Load players
    for(int i=1;i<=MAX_PLAYERS;++i) {
        string nodename = "player" + boost::lexical_cast<string>(i);
        if(node.hasNode(nodename))
            hs.addShip(parseShipConf(i, node.at(nodename)));
    }

    // Level
    hs.setLevel(node.at("level").value());

    return hs;
}

void Hotseat::addShip(const ShipConf &shipconf)
{
    m_ships.push_back(shipconf);
}

void Hotseat::initialize(World &world) const
{
    // Load level
    level::LevelRegistry::get(m_level).load(world);

    // Add ships
    for(const ShipConf &sc : m_ships) {
        Ship ship = Ship::make(sc);
        if(sc.position.hasValue())
            ship.physics().setPosition(sc.position.get());

        world.addShip(ship);
    }
}

GameInitError::GameInitError(const string &error)
: m_error(error)
{
}

GameInitError::~GameInitError() throw()
{
}

const char *GameInitError::what() const throw()
{
    return m_error.c_str();
}

std::ostream &operator<<(std::ostream &os, const GameInitError &ex)
{
    os << "Game initialization error: " << ex.m_error;
    return os;
}

}
