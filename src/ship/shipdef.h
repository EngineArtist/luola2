#ifndef LUOLA_SHIP_DEFINITION_H
#define LUOLA_SHIP_DEFINITION_H

#include <string>
#include <unordered_map>

class DataFile;
class ModelResource;
class ResourceLoader;

namespace conftree { class Node; }

/**
 * Ship definition.
 *
 * This defines the properties of the ship hull, as well as limits on
 * the equipment it can take.
 */
class ShipDef {
public:
    /**
     * Construct a new ship definition from configuration file.
     *
     * @param resloader the resource loader with which to load dependencies
     * @param doc configuration file root node
     */
    ShipDef(ResourceLoader &resloader, const conftree::Node &doc);

    /**
     * Get the short (player visible) name of the ship.
     *
     * This is the name that is usually displayed to the player.
     * 
     * @return ship name
     */
    const string &shortname() const { return m_shortname; }

    /**
     * Get the full (player visible) name of the ship.
     *
     * This is the (optional) full or unabbreviated name of the ship.
     * If not explicitly set, it will be the same as the short name.
     *
     * The long name is typically shown during ship selection as
     * additional flavor text.
     *
     * @return full ship name
     */
    const string &fullname() const { return m_fullname; }

    /**
     * Get the mass of the ship hull
     *
     * @return hull mass
     */
    float mass() const { return m_mass; }

    /**
     * Get the ship radius
     *
     * @return ship radius
     */
    float radius() const { return m_radius; }

    /**
     * Get the turning rate of the ship
     *
     * The turning rate is typically expressed as
     * degrees/s in the configuration file, but internally
     * it is converted to radians.
     * 
     * @return turning rate in radians per second
     */
    float turningRate() const { return m_turnrate; }

    /**
     * Get the ship's model
     *
     * @return model resource
     */
    const ModelResource *model() const { return m_model; }

private:
    ModelResource *m_model;

    string m_shortname, m_fullname;

    float m_mass;
    float m_radius;
    float m_turnrate;
};

/**
 * Ship definition collection
 */
class ShipDefs {
public:
    ShipDefs() = default;
    ShipDefs(const ShipDefs&) = delete;

    /**
     * Load all ships listed in the given file to memory
     *
     * The file should contain a list of names. The data files
     * "name.ship"  will then be searched for ship configurations
     * and assets.
     * 
     * In case of error, ShipDefException will be thrown.
     * 
     * @param name name of the ship to load
     */
    static void loadAll(DataFile &datafile, const string &filename);

    /**
     * Get the named ship definition.
     *
     * If no such ship has been defined, ShipDefException
     * will be thrown
     *
     * @param name ship name
     * @return ship definition
     */
    static const ShipDef *get(const string &name);

private:
    static ShipDefs &getInstance();
    static void load(const string &shipname);

    std::unordered_map<string, ShipDef*> m_shipdefs;
};


#endif
