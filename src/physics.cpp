#include <iostream>

#include "physics.h"
#include "world.h"

const float Physical::TPS = 60.0f;
const float Physical::TIMESTEP = 1.0f / Physical::TPS;

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

Physical::Physical(float mass, float radius, const glm::vec2 &pos, const glm::vec2 &vel)
    : m_pos(pos), m_vel(vel), m_mass(mass), m_radius(radius)
{
}

void Physical::step(const World &world)
{
    // Apply impulse and reset accumulator
    m_vel += m_imp * imass();
    m_imp = glm::vec2();

    // Integrate forces (RK4)
    Derivate a = evaluate(*this, 0.0f, Derivate());
    Derivate b = evaluate(*this, TIMESTEP*0.5f, a);
    Derivate c = evaluate(*this, TIMESTEP*0.5f, b);
    Derivate d = evaluate(*this, TIMESTEP, c);

    const glm::vec2 dposdt = 1.0f/6.0f * (a.dpos + 2.0f*(b.dpos + c.dpos) + d.dpos);
    const glm::vec2 dveldt = 1.0f/6.0f * (a.dvel + 2.0f*(b.dvel + c.dvel) + d.dvel);

    // Position and velocity steps
    glm::vec2 dpos = dposdt * TIMESTEP;
    m_vel += dveldt * TIMESTEP;

    // Check for terrain collisions
    terrain::Point cp;
    glm::vec2 cnorm;
    if(world.checkTerrainCollision(m_pos, m_radius, dpos, cp, cnorm)) {
        // Collision detected!
        std::cerr << "terrain collision (" << m_pos.x << "," << m_pos.y << ") detected at " << cp.x << ", " << cp.y << " [" << cnorm.x << "," << cnorm.y << "]" << std::endl;
        m_pos = cp;
        m_vel = glm::vec2(0, 0.01);
        //m_vel = glm::reflect(m_vel, cnorm);

    } else {
        // No collisions, apply position step normally
        m_pos += dposdt * TIMESTEP;
    }
}

bool Physical::checkCollision(Physical &other)
{
    // A collision occurs when distance between
    // the center of this object and other is <= this radius + other radius.
    glm::vec2 distv = position() - other.position();
    float dd = glm::dot(distv, distv);
    float r = radius() + other.radius();
    if(dd > r*r)
        return false;

    // Collision normal vector
    glm::vec2 normal = glm::normalize(distv);

    // Combined velocity
    glm::vec2 collv = velocity() - other.velocity();
    float impact_speed = glm::dot(collv, normal);

    // Check if objects are moving away from each other already
    if(impact_speed  > 0)
        return true;

    // Coefficient of restitution
    const float cor = 0.95f;

    // Collision impulse
    float j = -(1.0f + cor) * impact_speed / (imass() + other.imass());
    glm::vec2 impulse = j * normal;

    addImpulse(impulse);
    other.addImpulse(-impulse);

    return true;
}
