#include <stdexcept>

#include "../util/conftree.h"
#include "../ship/exception.h"
#include "../res/model.h"
#include "projectiledef.h"

ProjectileDef::ProjectileDef(const conftree::Node &node)
{
    m_mass = node.at("mass").floatValue();
    m_radius = node.at("radius").floatValue();
    m_model = resource::get<resource::Model>(node.at("model").value());
}

ProjectileDef::~ProjectileDef()
{
}

Projectile ProjectileDef::make(const glm::vec2 &pos, const glm::vec2 &vel) const
{
    return Projectile(this, pos, vel);
}

ProjectileFactoryBase::~ProjectileFactoryBase()
{
}

void Projectiles::loadAll(fs::DataFile &df, const string &filename)
{
    conftree::Node node = conftree::parseYAML(df, filename);

    Projectiles &p = getInstance();
    for(const string &key : node.itemSet()) {
        const conftree::Node &n = node.at(key);
        p.m_projectiles[key] = p.m_factories.at(n.at("codebase").value())->make(n);
    }
}

const ProjectileDef *Projectiles::get(const string &name)
{
    const Projectiles &p = getInstance();
    try {
        return p.m_projectiles.at(name);
    } catch(const std::out_of_range &ex) {
        throw ShipDefException("projectile \"" + name + "\" not defined!");
    } 
}

void Projectiles::registerFactory(const string &name, ProjectileFactoryBase *factory)
{
    m_factories[name] = factory;
}

namespace {
    Projectiles *PROJECTILES;
}

Projectiles &Projectiles::getInstance()
{
    if(!PROJECTILES)
        PROJECTILES = new Projectiles();
    return *PROJECTILES;
}
