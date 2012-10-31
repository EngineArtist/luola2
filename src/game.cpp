#include <iostream>
#include <random>

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

void bounds(const glm::vec2 pos, glm::vec2 &vel) {
    if(pos.x<-10 || pos.x>10)
        vel.x = -vel.x;
    if(pos.y<-7.5 || pos.y>7.5)
        vel.y = -vel.y;
}

void gameloop()
{
    glfwEnable( GLFW_STICKY_KEYS );
    glFrontFace(GL_CW);

    {
        DataFile df("core.data");
        if(df.isError()) {
            std::cerr << df.errorString() << "\n";
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

    static const int OBJS = 3;
    Physical objects[OBJS];
    for(int i=0;i<OBJS;++i) {
        objects[i].setPosition(glm::vec2(rand(generator), rand(generator)));
        objects[i].setVelocity(glm::vec2(rand(generator), rand(generator)) * 20.0f);
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
                objects[i].step();

                // Boundary collisions
                glm::vec2 vel = objects[i].velocity();
                bounds(objects[i].position(), vel);
                objects[i].setVelocity(vel);
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

        // Graphics
        glClear( GL_COLOR_BUFFER_BIT );
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
