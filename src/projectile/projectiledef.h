#ifndef LUOLA_PROJECTILE_DEF_H
#define LUOLA_PROJECTILE_DEF_H

#include <string>
#include <unordered_map>

using std::string;

#include "projectile.h"
#include "../res/mesh.h"

namespace conftree { class Node; }
namespace resource { class Model; }
namespace fs { class DataFile; }

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

    /**
     * Get the submesh of the projectile model to use for rendering
     *
     * @return mesh slice
     */
    resource::MeshSlice mesh() const { return m_mesh; }

    /**
     * Create projectile
     *
     * @param pos initial position
     * @param vel inherited velocity
     */
    virtual Projectile make(const glm::vec2 &pos, const glm::vec2 &vel) const;

private:
    float m_mass;
    float m_radius;

    resource::MeshSlice m_mesh;
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
    static void loadAll(fs::DataFile &df, const string &filename);

    /**
     * Get the named projectile definition
     *
     * If not found, ShipDefException will be thrown
     *
     * @param name projectile name
     * @return shared projectile instance
     */
    static const ProjectileDef *get(const string &name);

    /**
     * Get the shared Model to use for rendering projectiles.
     * 
     * @return model instance
     */
    static const resource::Model *getModel();

    /**
     * Set the model to use for rendering projectiles.
     *
     * Note. The model resource must have already been loaded at this point.
     *
     * @param name model resource name
     */
    static void setModel(const string &name);
 
    static Projectiles &getInstance();

    void registerFactory(const string &name, ProjectileFactoryBase *factory);

private:
    std::unordered_map<string, ProjectileDef*> m_projectiles;
    std::unordered_map<string, ProjectileFactoryBase*> m_factories;

    const resource::Model *m_model;
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

