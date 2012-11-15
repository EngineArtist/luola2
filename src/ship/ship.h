#ifndef LUOLA_SHIP_H
#define LUOLA_SHIP_H

#include "../physics.h"

class ModelResource;
class ShipDef;
class Engine;

class Ship {
public:
    /**
     * Construct a ship
     *
     * @param def ship definition
     * @param engine ship engine
     */
    Ship(const ShipDef &def, const Engine &engine);

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
     * Turn the ship.
     *
     * The turning angle is turning rate / physics step length.
     * 
     * @param right if true, turn to the right
     */
    void turn(bool right);

    /**
     * Apply engine thrust
     *
     * The thrust impulse is engine thrust / s / physics step length.
     */
    void thrust();

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

    const Engine &m_engine;

    const ModelResource *m_model;
};

#endif

