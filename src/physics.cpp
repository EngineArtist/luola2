#include "physics.h"

const float Physical::TIMESTEP = 1.0f / 60.0f;

namespace {
    struct State
    {
        glm::vec2 pos;
        glm::vec2 vel;
    };

    struct Derivate
    {
        glm::vec2 dpos;
        glm::vec2 dvel;
    };

    glm::vec2 acceleration(const glm::vec2 &pos, const glm::vec2 &vel)
    {
        return glm::vec2(0, -9.81);
    }

    Derivate evaluate(const Physical &obj, float dt, const Derivate &d)
    {
        glm::vec2 pos = obj.position() + d.dpos * dt;
        glm::vec2 vel = obj.velocity() + d.dvel * dt;

        Derivate out;
        out.dpos = vel;
        out.dvel = acceleration(pos, vel);

       return out;
    }
}

void Physical::step()
{
    Derivate a = evaluate(*this, 0.0f, Derivate());
    Derivate b = evaluate(*this, TIMESTEP*0.5f, a);
    Derivate c = evaluate(*this, TIMESTEP*0.5f, b);
    Derivate d = evaluate(*this, TIMESTEP, c);

    const glm::vec2 dposdt = 1.0f/6.0f * (a.dpos + 2.0f*(b.dpos + c.dpos) + d.dpos);
    const glm::vec2 dveldt = 1.0f/6.0f * (a.dvel + 2.0f*(b.dvel + c.dvel) + d.dvel);

    m_pos += dposdt * TIMESTEP;
    m_vel += dveldt * TIMESTEP;
}

