#include "physics.h"

const float Physical::TIMESTEP = 1.0f / 60.0f;

namespace {
    struct Derivate
    {
        glm::vec2 dpos;
        glm::vec2 dvel;
    };

    glm::vec2 acceleration(const glm::vec2 &vel, float drag, float mass)
    {
        // Gravity + air resistance
        // TODO + lift + gravity anomalies
        return glm::vec2(0, -9.81)
            - (glm::normalize(vel) * drag * glm::dot(vel, vel) * mass)
            ;
    }

    Derivate evaluate(const Physical &obj, float dt, const Derivate &d)
    {
        glm::vec2 vel = obj.velocity() + d.dvel * dt;

        // Drag coefficient.
        // TODO this depends on the object radius and the environment (i.e. water or air)
        float drag = 0.1;

        Derivate out;
        out.dpos = vel;
        out.dvel = acceleration(vel, drag, obj.mass());

       return out;
    }
}

Physical::Physical()
    : m_mass(1.0f), m_radius(1.0f)
{
}

Physical::Physical(const glm::vec2 &pos, float mass, float radius)
    : m_pos(pos), m_vel(glm::vec3(0.0f)), m_mass(mass), m_radius(radius)
{
}

void Physical::step()
{
    // Apply impulse and reset accumulator
    m_vel += m_imp / m_mass;
    m_imp = glm::vec2();

    // Integrate forces
    Derivate a = evaluate(*this, 0.0f, Derivate());
    Derivate b = evaluate(*this, TIMESTEP*0.5f, a);
    Derivate c = evaluate(*this, TIMESTEP*0.5f, b);
    Derivate d = evaluate(*this, TIMESTEP, c);

    const glm::vec2 dposdt = 1.0f/6.0f * (a.dpos + 2.0f*(b.dpos + c.dpos) + d.dpos);
    const glm::vec2 dveldt = 1.0f/6.0f * (a.dvel + 2.0f*(b.dvel + c.dvel) + d.dvel);

    m_pos += dposdt * TIMESTEP;
    m_vel += dveldt * TIMESTEP;
}
