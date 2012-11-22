#ifndef LUOLA_SHIP_H
#define LUOLA_SHIP_H

#include <array>
#include <string>
#include <vector>

using std::string;

#include "../physics.h"

class World;
class ShipDef;
class Engine;
class PowerPlant;
class Equipment;
class Weapon;
class ProjectileDef;

namespace resource { class Model; }
namespace gameinit {
    class WeaponConf;
    class ShipConf;
}

/**
 * Maximum number of weapons that will fit on the ship.
 *
 * The game engine /should/ be weapon count agnostic.
 * Should we ever decide to support more (or less) weapons, this constant
 * (and the weapon selection UI and trigger input mappings)
 * should be the only things that need changing.
 */
static const int MAX_WEAPONS = 2;

/**
 * A ship's weapon type and status.
 *
 * For weapon types with no customizable ammo, the projectile
 * field should be null.
 */
struct ShipWeapon {
    //! The weapon
    const Weapon *weapon;

    //! Projectile type for launcher weapons
    const ProjectileDef *projectile;

    //! Weapon cooloff timer
    int cooloff;
};
typedef std::array<ShipWeapon, MAX_WEAPONS> ShipWeapons;

class Ship {
public:
    Ship() = default;

	/**
	 * Make a ship with the given customizations.
	 *
	 * @param conf ship configuration
	 * @return ship instance
	 */
    static Ship make(const gameinit::ShipConf &conf);

    /**
     * Get the physics object of the ship
     * @return physics
     */
    Physical &physics() { return m_physics; }
    const Physical &physics() const { return m_physics; }

    /**
     * Add a batteryG to the ship
     *
     * @param capacity battery capacity
     * @param chargerate battery charge rate
     */
    void addBattery(float capacity, float chargerate);

    /**
     * Draw the ship
     * 
     * @param transform
     */
    void draw(const glm::mat4 &transform) const;

    /**
     * Get the currently stored amount of energy
     *
     * @return available energy
     */
    float energy() const { return m_energy; }

    /**
     * Get the maximum available energy
     *
     * @return maximum available energy
     */
    float maxEnergy() const { return m_maxenergy; }

    /// Turning directions
    enum TurnDir { NOTURN, CW, CCW };

    /**
     * Turn the ship.
     *
     * The turning angle is turning rate / physics step length.
     * 
     * @param direction turning direction
     */
    void turn(TurnDir direction);

    /**
     * Turn engine on/off
     *
     * The thrust impulse is engine thrust / s / physics step length.
     */
    void thrust(bool on);

    /**
     * Trigger/untrigger weapon.
     *
     * @param weapon the weapon number. Must be in range [1..MAX_WEAPONS]
     * @param on trigger state
     */
    void trigger(int weapon, bool on);

    /**
     * Perform ship simulation step.
     *
     * This runs the ship's internal systems (engine, turning, weapons, etc.)
     * Physics simulation is done by calling physics().step()
     *
     * @param world the game state
     */
    void shipStep(World &world);

    /**
     * Set the direction the ship's nose is pointing at.
     *
     * Angle should be in range [0..2pi]
     * @param a new angle
     */
    void setAngle(float a);

private:
    /**
     * Construct a ship
     *
     * @param def ship definition
     * @param engine ship engine
     */
    Ship(const ShipDef *def,
         const Engine *engine,
         const PowerPlant *power,
         const std::vector<const Equipment*> &equipment,
         const ShipWeapons &weapons
        );

    Physical m_physics;

    float m_angle;
    float m_turnrate;
    glm::vec2 m_thrustimpulse;

    // current charge level
    float m_energy, m_maxenergy;
    // how much energy can be stored for later use
    float m_battery;
    // maximum battery recharge rate
    float m_battery_charge_rate;

    TurnDir m_state_turn;
    bool m_state_thrust;
    bool m_state_trigger[MAX_WEAPONS];

    const Engine *m_engine;
    const PowerPlant *m_power;
    ShipWeapons m_weapons;

    const resource::Model *m_model;
};

#endif

