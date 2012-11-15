#include <iostream>

#include "world.h"
#include "ship/ship.h"

void World::step()
{
    for(unsigned int i=0;i<m_ships.size();++i) {
        Physical &obj = m_ships[i]->physics();

        m_ships[i]->shipStep();
        obj.step(*this);

        // Boundary collisions
        glm::vec2 vel = obj.velocity();

        if(obj.position().x<-10 || obj.position().x>10)
            vel.x = -vel.x;
        if(obj.position().y<-7.5 || obj.position().y>7.5)
            vel.y = -vel.y;

        obj.setVelocity(vel);

        // Object-object collisions
        for(unsigned int j=i+1;j<m_ships.size();++j) {
            if(obj.checkCollision(m_ships[j]->physics())) {
                std::cout << "collision " << i << "--" << j << std::endl;
            }
        }
    }
}

terrain::ZoneProps World::zoneAt(const terrain::Point &p) const
{
    terrain::ZoneProps zp = m_rootzone;

    // A point can be part of multiple zones
    for(const terrain::Zone *z : m_zones) {
        if(z->hasPoint(p))
            z->apply(zp);
    }

    return zp;
}

bool World::checkTerrainCollision(const terrain::Point &p, float r, const glm::vec2 &v, terrain::Point &cp, glm::vec2 &normal) const
{
    for(const terrain::Solid *s : m_dyn_terrain) {
        if(s->circleCollision(p, r, v, cp, normal))
            return true;
    }

    for(const terrain::Solid *s : m_static_terrain) {
        if(s->circleCollision(p, r, v, cp, normal))
            return true;
    }

    return false;
}

void World::addShip(Ship *ship)
{
    assert(ship);
    m_ships.push_back(ship);
}

void World::addZone(terrain::Zone *zone)
{
    assert(zone);
    m_zones.push_back(zone);
    zone->updateGl();
}

void World::addSolid(terrain::Solid *solid)
{
    assert(solid);
    m_dyn_terrain.push_back(solid);
    solid->updateGl();
}

void World::addStaticSolid(terrain::Solid *solid)
{
    assert(solid);
    m_static_terrain.push_back(solid);
    solid->updateGl();
}

void World::makeHole(const terrain::ConvexPolygon &hole)
{
    for(terrain::Solid *s : m_dyn_terrain) {
        // TODO update GL only just before we need to render
        if(s->nibble(hole))
            s->updateGl();
    }
}