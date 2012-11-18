#include <ostream>

#include "util/conftree.h"

#include "launcher.h"
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

    ShipConf parseShipConf(const conftree::Node &node)
    {
        return {
            node.at("hull").value(),
            node.at("power").value(),
            node.at("engine").value(),
            node2vec(node.opt("equipment")),
            node2optvec2(node.opt("position"))
        };
    }
}

Hotseat Hotseat::loadFromFile(const string &filename)
{
    conftree::Node node = conftree::parseYAML(filename);
    Hotseat hs;

    // Load players
    static const char *PLAYERS[] = {"player1", "player2", "player3", "player4"};
    for(unsigned int i=0;i<4;++i) {
        if(node.hasNode(PLAYERS[i]))
            hs.addShip(i+1, parseShipConf(node.at(PLAYERS[i])));
    }

    return hs;
}

void Hotseat::addShip(int player, const ShipConf &shipconf)
{
    if(player < 1 || player > 4)
        throw GameInitError("Local player number must be in the range [1..4]");

    m_ships.push_back(std::make_pair(player, shipconf));
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
    for(const std::pair<int, ShipConf> &psc : m_ships) {
        int player = psc.first;
        const ShipConf &sc = psc.second;

        Ship *ship = Ship::make(sc.hull, sc.power, sc.engine, sc.equipment);
        if(sc.position.hasValue())
            ship->physics().setPosition(sc.position.get());

        world.addShip(player, ship);
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