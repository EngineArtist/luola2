#ifndef LUOLA_WEAPON_H
#define LUOLA_WEAPON_H

#include <string>
#include <unordered_map>

using std::string;

namespace conftree { class Node; }
namespace fs { class DataFile; }

class Ship;
class World;
class ProjectileDef;

class Weapon {
public:
    Weapon(const conftree::Node &node);
    virtual ~Weapon();

    /**
     * Return the number of physics ticks this weapon requires to cool off
     * between shots.
     *
     * @return number of ticks
     */
    int cooloffPeriod() const { return m_cooloff; }

    virtual void fire(const ProjectileDef *projectile, Ship &ship, World &world) const = 0;

private:
    int m_cooloff;
};

class WeaponFactoryBase
{
public:
    virtual ~WeaponFactoryBase();
    virtual Weapon *make(const conftree::Node &node) const = 0;
};

class Weapons
{
public:
    Weapons() { }
    Weapons(const Weapons&) = delete;

    /**
     * Load all weapon definitions from the data file
     * 
     * @param df the data file
     * @param filename weapon definition file name
     */
    static void loadAll(fs::DataFile &df, const string &filename);

    /**
     * Get the named weapon
     *
     * If not found, ShipDefException will be thrown
     *
     * @param name weapon name
     * @return shared weapon instance
     */
    static const Weapon *get(const string &name);

    static Weapons &getInstance();

    void registerFactory(const string &name, WeaponFactoryBase *factory);

private:
    std::unordered_map<string, Weapon*> m_weapons;
    std::unordered_map<string, WeaponFactoryBase*> m_factories;
};

template<class W>
class WeaponFactory : public WeaponFactoryBase {
public:
    WeaponFactory(const string &name)
    {
        Weapons::getInstance().registerFactory(name, this);
    }

    Weapon *make(const conftree::Node &node) const
    {
        return new W(node);
    }
};

#endif

