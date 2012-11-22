#ifndef LUOLA_SHIP_EQUIPMENT_H
#define LUOLA_SHIP_EQUIPMENT_H

#include <string>
#include <unordered_map>

using std::string;

namespace conftree { class Node; }
namespace fs { class DataFile; }

class Ship;

/**
 * Extra ship equipment.
 *
 * Equipment add or modify ship features, but unlike engines and power plants,
 * multiple equipment can be added (or none at all) and unlike weapons,
 * equipment are not triggered.
 */
class Equipment {
public:
    Equipment(const conftree::Node &node);
    virtual ~Equipment();

    /**
     * Modify the ship's properties.
     * This is called once during the ship's initialization.
     *
     * @param ship ship to modify
     */
    virtual void applyModification(Ship &ship) const = 0;

    /**
     * Is this an active equipment?
     *
     * Active equipments are kept around after initialization
     * and can perform actions every simulation tick.
     * 
     * @return true if the equipment has active functionality
     */
    virtual bool isActive() const { return false; }
};

/**
 * Base class for equipment factories.
 *
 * Use the EquipmentFactory template class to automatically
 * register your Equipment classes.
 */
class EquipmentFactoryBase
{
public:
    virtual ~EquipmentFactoryBase();

    virtual Equipment *make(const conftree::Node &node) const = 0;
};

class Equipments {
public:
    Equipments() { }
    Equipments(const Equipments&) = delete;

    /**
     * Load all equipment definitions from the data file
     *
     * @param df the datafile
     * @param filename equipment definition file name
     */
    static void loadAll(fs::DataFile &df, const string &filename);

    /**
     * Get the named equipment
     *
     * If not found, ShipDefException will be thrown.
     *
     * @param name equipment name
     * @return shared equipment instance
     */
    static const Equipment *get(const string &name);

    static Equipments &getInstance();

    void registerFactory(const string &name, EquipmentFactoryBase *factory);

private:
    std::unordered_map<string, Equipment*> m_equipment;
    std::unordered_map<string, EquipmentFactoryBase*> m_factories;
};

template<class Eq>
class EquipmentFactory : public EquipmentFactoryBase {
public:
    EquipmentFactory(const string &name)
    {
        Equipments::getInstance().registerFactory(name, this);
    }

    Equipment *make(const conftree::Node &node) const
    {
        return new Eq(node);
    }
};

#endif

