#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#include "../res/model.h"

#include "shipdef.h"
#include "ship.h"
#include "engine.h"
#include "power.h"

Ship::Ship(const ShipDef &def, const Engine &engine, const PowerPlant &power)
    : m_turnrate(def.turningRate() * Physical::TIMESTEP),
      m_engine(engine),
      m_power(power),
      m_model(def.model())
{
    m_physics.setRadius(def.radius());
    m_physics.setMass(def.mass());
    m_physics.setPosition(glm::vec2(0, 0));
    m_physics.setVelocity(glm::vec2(0, 1));

    m_battery = 0.0f;
    m_battery_charge_rate = 0.0f;

    m_maxenergy = power.energy() / Physical::TIMESTEP + m_battery;
    m_energy = m_maxenergy;

    setAngle(0.0f);
}

void Ship::draw(const glm::mat4 &transform) const
{
    static const glm::vec3 axis(0, 0, 1);

    glm::mat4 m = glm::rotate(
        glm::translate(
            transform,
            glm::vec3(m_physics.position(), 0.0f)),
        glm::degrees(m_angle) - 90,
        axis);

    m_model->render(m);
}

void Ship::setAngle(float a)
{
    assert(0 <= a && a <= 2*M_PI);

    m_angle = a;
    m_thrustimpulse = glm::vec2(
        cos(a) * m_engine.thrust(),
        sin(a) * m_engine.thrust()
        );
}

void Ship::turn(TurnDir dir)
{
    m_state_turn = dir;
}

void Ship::thrust(bool on)
{
    m_state_thrust = on;
}

void Ship::shipStep()
{
    // Power generation
    float energy = m_energy + m_power.energy();

    // Turning
    float a = m_angle;
    switch(m_state_turn) {
        case NOTURN: break;
        case CW:
            a -= m_turnrate;
            if(a < 0)
                a = 2*M_PI + a;
            setAngle(a);
            break;
        case CCW:
            a += m_turnrate;
            if(a > 2*M_PI)
                a = a - 2*M_PI;
            setAngle(a);
            break;
    }

    // Engine
    if(m_state_thrust) {
        float e = m_engine.thrustEnergy();
        if(energy >= e) {
            m_physics.addImpulse(m_thrustimpulse);
            energy -= e;
        }
    } else {
        energy -= m_engine.idleEnergy();
    }

    // Charge battery with leftover energy
    float de = std::min(energy - m_energy, m_battery_charge_rate);
    m_energy = std::max(std::min(m_energy + de, m_battery), 0.0f);
}
