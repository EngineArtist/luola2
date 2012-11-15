#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#include "../res/model.h"

#include "shipdef.h"
#include "ship.h"
#include "engine.h"

Ship::Ship(const ShipDef &def, const Engine &engine)
    : m_turnrate(def.turningRate() * Physical::TIMESTEP),
      m_engine(engine),
      m_model(def.model())
{
    m_physics.setRadius(def.radius());
    m_physics.setMass(def.mass());
    m_physics.setPosition(glm::vec2(0, 0));
    m_physics.setVelocity(glm::vec2(0, 1));

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

void Ship::turn(bool cw)
{
    float a;
    if(cw) {
        a = m_angle - m_turnrate;
        if(a < 0)
            a = 2*M_PI - m_angle;
    } else {
        a = m_angle + m_turnrate;
        if(a > 2*M_PI)
            a = 2*M_PI;
    }
    setAngle(a);
}

void Ship::thrust()
{
    m_physics.addImpulse(m_thrustimpulse);
}
