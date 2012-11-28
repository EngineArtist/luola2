//
// This file is part of Luola2.
// Copyright (C) 2012 Calle Laakkonen
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
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#include "../res/model.h"
#include "../gameinit.h"

#include "shipdef.h"
#include "ship.h"
#include "engine.h"
#include "power.h"
#include "../equipment/equipment.h"
#include "../weapon/weapon.h"
#include "../projectile/projectiledef.h"

Ship::Ship(int player, int team,
           const ShipDef *def,
           const Engine *engine,
           const PowerPlant *power,
           const std::vector<const Equipment*> &equipment,
           const ShipWeapons &weapons
          )
    : m_player(player), m_team(team),
      m_turnrate(def->turningRate() * Physical::TIMESTEP),
      m_engine(engine),
      m_power(power),
      m_weapons(weapons),
      m_model(def->model())
{
    // Set physical properties
    m_physics.setRadius(def->radius());
    m_physics.setMass(def->mass());
    m_physics.setPosition(glm::vec2(0, 0));
    m_physics.setVelocity(glm::vec2(0, 1));

    // Battery
    m_battery = 0.0f;
    m_battery_charge_rate = 0.0f;

    // Add equipment
    for(const Equipment *eq : equipment) {
        eq->applyModification(*this);
    }

    // Calculate cached values
    m_maxenergy = power->energy() + m_battery;
    m_energy = m_maxenergy;

    setAngle(0.0f);
}

Ship Ship::make(const gameinit::ShipConf &shipconf)
{
    const ShipDef *hull = ShipDefs::get(shipconf.hull);
    const Engine *engine = Engines::get(shipconf.engine);
    const PowerPlant *power = PowerPlants::get(shipconf.power);

    std::vector<const Equipment*> equipment;
    for(const string &eq : shipconf.equipment)
        equipment.push_back(Equipments::get(eq));

    ShipWeapons weapons;
    for(unsigned int i=0;i<weapons.size();++i) {
        if(shipconf.weapons[i].weapon.empty()) {
            weapons[i].weapon = nullptr;
            weapons[i].projectile = nullptr;
        } else {
            weapons[i].weapon = Weapons::get(shipconf.weapons[i].weapon);
            if(shipconf.weapons[i].ammo.empty())
                weapons[i].projectile = nullptr;
            else
                weapons[i].projectile = Projectiles::get(shipconf.weapons[i].ammo);
        }

        weapons[i].cooloff = 0;
    }

    return Ship(shipconf.player, shipconf.team, hull, engine, power, equipment, weapons);
}

void Ship::addBattery(float capacity, float chargerate)
{
    m_battery += capacity;
    m_battery_charge_rate = (m_battery_charge_rate + chargerate) / 2.0f;
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

    m_model->prepareRender();
    m_model->render(m);
}

void Ship::setAngle(float a)
{
    assert(0 <= a && a <= 2*M_PI);

    m_angle = a;
    m_thrustimpulse = glm::vec2(
        cos(a) * m_engine->thrust(),
        sin(a) * m_engine->thrust()
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

void Ship::trigger(int weapon, bool on)
{
    assert(weapon>0 && weapon <= MAX_WEAPONS);
    m_state_trigger[weapon-1] = on;
}

void Ship::shipStep(World &world)
{
    // Power generation
    float energy = m_energy + m_power->energy();

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
        float e = m_engine->thrustEnergy();
        if(energy >= e) {
            m_physics.addImpulse(m_thrustimpulse);
            energy -= e;
        }
    } else {
        energy -= m_engine->idleEnergy();
    }

    // Weapons
    for(unsigned int i=0;i<m_weapons.size();++i) {
        if(m_weapons[i].cooloff > 0) {
            --m_weapons[i].cooloff;
        } else if(m_state_trigger[i] && m_weapons[i].weapon && energy >= m_weapons[i].weapon->energy()) {
            m_weapons[i].weapon->fire(m_weapons[i].projectile, *this, world);
            m_weapons[i].cooloff = m_weapons[i].weapon->cooloffPeriod();
            energy -= m_weapons[i].weapon->energy();
        }
    }

    // Charge battery with leftover energy
    float de = std::min(energy - m_energy, m_battery_charge_rate);
    m_energy = std::max(std::min(m_energy + de, m_maxenergy), 0.0f);
}
