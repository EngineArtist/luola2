//
// This file is part of Luola2.
//
// Luola2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Luola2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Luola2.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef LUOLA_PHYSICS
#define LUOLA_PHYSICS

#include <glm/glm.hpp>

class World;

/**
 * A physical object.
 */
class Physical {
public:
    /**
     * Ticks Per Second.
     *
     * This is the number of simulation steps taken in one second.
     */
    static const float TPS;

    /**
     * delta T
     *
     * This is the length (in seconds) of a single physics tick.
     */
    static const float TIMESTEP;

    /**
     * Default constructor
     */
    Physical();

    /**
     * Construct a new physical object
     *
     * @param pos initial position
     * @param mass mass
     * @param radius radius
     */
    Physical(float mass, float radius, const glm::vec2 &pos, const glm::vec2 &vel);

    /**
     * Get the position of the object
     *
     * @return position vector
     */
    const glm::vec2 &position() const { return m_pos; }

    /**
     * Set the position of the object
     *
     * @param pos new position
     */
    void setPosition(const glm::vec2 pos) { m_pos = pos; }

    /**
     * Get the velocity of the object
     *
     * @return velocity vector
     */
    const glm::vec2 &velocity() const { return m_vel; }

    /**
     * Set the velocity of the object.
     *
     * Note. Typically you should use the addImpulse function
     * to change the velocity. Use this only for initialization
     * and when you need to break the laws of physics.
     *
     * @param vel new velocity
     */
    void setVelocity(const glm::vec2 &vel) { m_vel = vel; }

    /**
     * Add an impulse to the impulse accumulator.
     *
     * The total impulse will be applied on the next simulation step.
     *
     * @param force impulse force
     */
    void addImpulse(const glm::vec2 &impulse) { m_imp += impulse; }

    /**
     * Get the impulse accumulator
     *
     * The impulse accumulator holds the impulse force
     * that will be applied to the object at the integration step.
     *
     * @return accumulated impulse
     */
    const glm::vec2 impulse() const { return m_imp; }

    /**
     * Get the mass of the object
     *
     * @return mass
     */
    float mass() const { return m_mass; }

    /**
     * Get the mass⁻¹
     *
     * This is just a shortcut for certain calculations.
     *
     * @return inverse mass
     */
    float imass() const { return 1.0f / m_mass; }

    /**
     * Change the mass of the object
     *
     * @param mass new mass
     */
    void setMass(float mass) { m_mass = mass; }

    /**
     * Get the radius of the object.
     *
     * The radius is used for collision detection and air
     * resistance calculation.
     *
     * @return object radius
     */
    float radius() const { return m_radius; }

    /**
     * Set the radius of the object.
     *
     * The radius must be greater than zero.
     *
     * @param radius object radius
     */
    void setRadius(float radius) { assert(radius>0); m_radius = radius; }

    /**
     * Simulate a timestep.
     *
     */
    void step(const World &world);

    /**
     * Check if this object is currently colliding with the other object.
     *
     * @param object the other object to check
     * @return true if this object is in collision with the other one.
     */
    bool checkCollision(Physical &other);

private:
    // Position
    glm::vec2 m_pos;

    // Velocity
    glm::vec2 m_vel;

    // Impulse accumulator
    glm::vec2 m_imp;

    float m_mass;
    float m_radius;
};

#endif

