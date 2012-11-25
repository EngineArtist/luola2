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

terrain::ConvexPolygon makeNibblePolygon(float x, float y)
{
    using namespace terrain;
    std::vector<Point> points;
#if 1
    for(int i=0;i<6;++i) {
        points.push_back(Point(cos(i/6.0 * M_PI * 2) * 0.5 + x, sin(i/6.0 * M_PI * 2) * 0.5 + y));
    }
#else
    points.push_back(Point(x-0.5, y+1));
    points.push_back(Point(x-0.5, y-1));
    points.push_back(Point(x+0.5, y-1));
    points.push_back(Point(x+0.5, y+1));
#endif
    return ConvexPolygon(points);
}

void gameloop(const gameinit::Hotseat &init)
{
    glfwEnable( GLFW_STICKY_KEYS );
    glFrontFace(GL_CW);

    World world;
    Renderer renderer(world);

    init.initialize(world);

    input::initPlayerInputs();

    double time_now = glfwGetTime();
    double time_accumulator = 0.0;

    bool mousedown = false;
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

        // Testing... To be removed.
        if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
            if(!mousedown) {
                mousedown = true;
                int mx, my;
                glfwGetMousePos(&mx, &my);
                float x, y;
                x = (mx - 400) / 400.0 * 10;
                y = -(my - 300) / 300.0 * 7.5;
                std::cout << "mouse " << mx << ", " << my << " --> " << x << ", " << y << std::endl;
                world.makeHole(makeNibblePolygon(x, y));
            }
        } else
            mousedown = false;

        // Graphics
        renderer.render(frame_time);
    } while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS && glfwGetWindowParam( GLFW_OPENED ) );

    input::deinitPlayerInputs();
}
