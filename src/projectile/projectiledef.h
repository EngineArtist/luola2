#ifndef LUOLA_PROJECTILE_DEF_H
#define LUOLA_PROJECTILE_DEF_H

#include <string>
#include <unordered_map>

using std::string;

namespace conftree { class Node; }

class DataFile;

/**
 * Projectile definition.
 *
 * This class acts as a factory for the physical projectile instances.
 */
class ProjectileDef {
public:
    ProjectileDef(const conftree::Node &node);
    virtual ~ProjectileDef();

    /**
     * The mass of the projectile
     * 
     * @return mass
     */
    float mass() const { return m_mass; }

    /**
     * The radius of the projectile
     *
     * @return radius
     */
    float radius() const { return m_radius; }

private:
    float m_mass;
    float m_radius;
};

class ProjectileFactoryBase
{
public:
    virtual ~ProjectileFactoryBase();
    virtual ProjectileDef *make(const conftree::Node &node) const = 0;
};

class Projectiles
{
public:
    Projectiles() = default;
    Projectiles(const Projectiles&) = delete;

    /**
     * Load all projectile definitions from the data file
     * 
     * @param df the data file
     * @param filename projectile definition file name
     */
    static void loadAll(DataFile &df, const string &filename);

    /**
     * Get the named projectile definition
     *
     * If not found, ShipDefException will be thrown
     *
     * @param name projectile name
     * @return shared projectile instance
     */
    static const ProjectileDef *get(const string &name);

    static Projectiles &getInstance();

    void registerFactory(const string &name, ProjectileFactoryBase *factory);

private:
    std::unordered_map<string, ProjectileDef*> m_projectiles;
    std::unordered_map<string, ProjectileFactoryBase*> m_factories;
};

template<class W>
class ProjectileFactory : public ProjectileFactoryBase {
public:
    ProjectileFactory(const string &name)
    {
        Projectiles::getInstance().registerFactory(name, this);
    }

    ProjectileDef *make(const conftree::Node &node) const
    {
        return new W(node);
    }
};

#endif

