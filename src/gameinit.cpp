#include <ostream>

#include <boost/lexical_cast.hpp>

#include "util/conftree.h"

#include "gameinit.h"
#include "world.h"
#include "ship/ship.h"
#include "terrain/terrains.h"

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

    return hs;
}

void Hotseat::addShip(const ShipConf &shipconf)
{
    m_ships.push_back(shipconf);
}

namespace {
terrain::Solid *makeTerrain()
{
    using namespace terrain;
    std::vector<ConvexPolygon> polys;
    std::vector<Point> points;
    #if 1
    for(int i=0;i<6;++i) {
        points.push_back(Point(cos(i/6.0 * M_PI * 2) * 3, sin(i/6.0 * M_PI * 2) * 3));
    }
    #else
    points.push_back(Point(-2, 1.2));
    points.push_back(Point(-2, -1.2));
    points.push_back(Point(3, -0));
    #endif
    polys.push_back(points);

    return new terrain::Solid(polys);
}
}

void Hotseat::initialize(World &world) const
{
    // Load level (TODO)
    world.addSolid(makeTerrain());

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
