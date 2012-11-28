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
#include <iostream>
#include <random>
#include <cmath>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "fs/datafile.h"
#include "res/loader.h"

#include "input.h"
#include "physics.h"
#include "game.h"
#include "gameinit.h"
#include "world.h"
#include "ship/shipdef.h"
#include "ship/ship.h"
#include "ship/engine.h"
#include "ship/power.h"
#include "equipment/equipment.h"
#include "renderer.h"

void gameloop(const gameinit::Hotseat &init)
{
    glfwEnable( GLFW_STICKY_KEYS );
    glFrontFace(GL_CW);

    World world;
    Renderer renderer(world);

    renderer.setZoom(15);

    init.initialize(world);

    input::initPlayerInputs();

    double time_now = glfwGetTime();
    double time_accumulator = 0.0;

    do {
        double new_time = glfwGetTime();
        double frame_time = new_time - time_now;
        if(frame_time > 0.25)
            frame_time = 0.25;
        time_now = new_time;

        time_accumulator += frame_time;

        // Interaction
        for(int i=1;i<=gameinit::Hotseat::MAX_PLAYERS;++i) {
            input::PlayerInput &pi = input::getPlayerInput(i);
            if(pi.isChanged()) {
                Ship *ship = world.getPlayerShip(i);
                if(ship) {
                    ship->thrust(pi.axisY() > 0);
                    if(pi.axisX() < 0)
                        ship->turn(Ship::CCW);
                    else if(pi.axisX() > 0)
                        ship->turn(Ship::CW);
                    else
                        ship->turn(Ship::NOTURN);

                    ship->trigger(1, pi.trigger1());
                    ship->trigger(2, pi.trigger2());
                }
                pi.clear();
            }
        }

        // Physics
        while(time_accumulator >= Physical::TIMESTEP) {
            world.step();
            time_accumulator -= Physical::TIMESTEP;
        }

        // Graphics
        renderer.setCenter(world.getPlayerShip(1)->physics().position());
        renderer.render(frame_time);
    } while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS && glfwGetWindowParam( GLFW_OPENED ) );

    input::deinitPlayerInputs();
}
