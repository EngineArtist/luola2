#ifndef LUOLA_SHIP_H
#define LUOLA_SHIP_H

#include "../physics.h"

class ModelResource;
class ShipDef;
class Engine;
class PowerPlant;

class Ship {
public:
    /**
     * Construct a ship
     *
     * @param def ship definition
     * @param engine ship engine
     */
    Ship(const ShipDef &def, const Engine &engine, const PowerPlant &power);

    /**
     * Get the physics object of the ship
     * @return physics
     */
    Physical &physics() { return m_physics; }
    const Physical &physics() const { return m_physics; }

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
     * Perform ship simulation step.
     *
     * This runs the ship's internal systems (engine, turning, weapons, etc.)
     * Physics simulation is done by calling physics().step()
     */
    void shipStep();

    /**
     * Set the direction the ship's nose is pointing at.
     *
     * Angle should be in range [0..2pi]
     * @param a new angle
     */
    void setAngle(float a);

private:
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

    const Engine &m_engine;
    const PowerPlant &m_power;

    const ModelResource *m_model;
};

#endif

