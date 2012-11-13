#include <iostream>
#include <random>
#include <cmath>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "fs/datafile.h"
#include "res/loader.h"
#include "res/model.h"
#include "res/font.h"

#include "physics.h"
#include "game.h"
#include "terrain/terrain.h"

void bounds(const glm::vec2 pos, glm::vec2 &vel) {
    if(pos.x<-10 || pos.x>10)
        vel.x = -vel.x;
    if(pos.y<-7.5 || pos.y>7.5)
        vel.y = -vel.y;
}

std::vector<terrain::ConvexPolygon> makeTerrain()
{
    using namespace terrain;
    std::vector<ConvexPolygon> polys;
    std::vector<Point> points;
#if 1
    for(int i=0;i<6;++i) {
        points.push_back(Point(cos(i/6.0 * M_PI * 2) * 3, sin(i/6.0 * M_PI * 2) * 3));
        std::cerr << points[points.size()-1].x << ", " << points[points.size()-1].y << std::endl;
    }
#else
    points.push_back(Point(-2, 1.2));
    points.push_back(Point(-2, -1.2));
    points.push_back(Point(3, -0));
#endif
    polys.push_back(points);

    return polys;
}

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

void gameloop()
{
    glfwEnable( GLFW_STICKY_KEYS );
    glFrontFace(GL_CW);

    {
        DataFile df("core.data");
        if(df.isError()) {
            std::cerr << "core data error: " << df.errorString() << "\n";
            return;
        }

        ResourceLoader rl(df, "resources.yaml");
        //font = static_cast<FontResource*>(rl.load("core.font.default"));
        // default font is autoloaded
    }
    FontResource *font = Resources::get<FontResource>("core.font.default");

    ModelResource *ship1;
    {
        DataFile df("test.ship");
        if(df.isError()) {
            std::cerr << df.errorString() << "\n";
            return;
        }

        ResourceLoader rl(df, "resources.yaml");
        ship1 = static_cast<ModelResource*>(rl.load("ship.model1"));
    }

    std::default_random_engine generator;
    std::uniform_real_distribution<float> rand(-1.0,1.0);

    static const int OBJS = 1;
    Physical objects[OBJS];
    for(int i=0;i<OBJS;++i) {
        objects[i].setPosition(glm::vec2(rand(generator), rand(generator)));
        objects[i].setVelocity(glm::vec2(rand(generator), rand(generator)) * 20.0f);
        objects[i].setRadius(0.2);
    }

    glm::mat4 proj = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    glm::mat4 view = glm::lookAt(
        glm::vec3(0,0,20),
        glm::vec3(0,0,0),
        glm::vec3(0,1,0)
    );

    glm::mat4 PV = proj * view;

    double time_now = glfwGetTime();
    double time_accumulator = 0.0;

    terrain::Terrain terrain(makeTerrain());
    terrain.updateGl();

    bool mousedown = false;
    do {
        double new_time = glfwGetTime();
        double frame_time = new_time - time_now;
        if(frame_time > 0.25)
            frame_time = 0.25;
        time_now = new_time;

        time_accumulator += frame_time;

        // Physics
        while(time_accumulator >= Physical::TIMESTEP) {
            for(int i=0;i<OBJS;++i) {
                objects[i].step(terrain);

                // Boundary collisions
                glm::vec2 vel = objects[i].velocity();
                bounds(objects[i].position(), vel);
                objects[i].setVelocity(vel);

                // Object-object collisions
                for(int j=i+1;j<OBJS;++j) {
                    if(objects[i].checkCollision(objects[j])) {
                        std::cout << "collision " << i << "--" << j << std::endl;
                    }
                }
            }
            time_accumulator -= Physical::TIMESTEP;
        }

        // Interaction
        if(glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS) {
            for(int i=0;i<OBJS;++i)
                objects[i].addImpulse(glm::vec2(0, 0.2));
        }
        if(glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS) {
            for(int i=0;i<OBJS;++i)
                objects[i].addImpulse(glm::vec2(-0.2, 0));
        }
        if(glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) {
            for(int i=0;i<OBJS;++i)
                objects[i].addImpulse(glm::vec2(0.2, 0));
        }

        if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
            if(!mousedown) {
                mousedown = true;
                int mx, my;
                glfwGetMousePos(&mx, &my);
                float x, y;
                x = (mx - 400) / 400.0 * 10;
                y = -(my - 300) / 300.0 * 7.5;
                std::cout << "mouse " << mx << ", " << my << " --> " << x << ", " << y << std::endl;
                bool dirty = terrain.nibble(makeNibblePolygon(x, y));
                if(dirty) {
                    std::cout << "terrain changed!" << std::endl;
                    terrain.updateGl();
                }
            }
        } else
            mousedown = false;

        // Graphics
        glClear( GL_COLOR_BUFFER_BIT );

        terrain.draw(PV);

        for(int i=0;i<OBJS;++i) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(objects[i].position().x, objects[i].position().y, 0));
            ship1->render(PV * model);
        }

        font->text("FPS: %.1f", 1.0 / frame_time)
            .scale(0.5).pos(1,1).align(TextRenderer::RIGHT).color(1,1,0)
            .render();

        glfwSwapBuffers();
    } while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS && glfwGetWindowParam( GLFW_OPENED ) );

    Resources::getInstance().unloadResource("ship.model1");
}
