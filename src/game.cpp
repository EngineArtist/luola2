#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "fs/datafile.h"
#include "res/loader.h"
#include "res/model.h"
#include "res/font.h"

#include "game.h"

void bounds(const glm::vec3 pos, glm::vec3 &vel) {
    if(pos.x<-10 || pos.x>10)
        vel.x = -vel.x;
    if(pos.y<-7.5 || pos.y>7.5)
        vel.y = -vel.y;
}

void gameloop()
{
    glfwEnable( GLFW_STICKY_KEYS );
    glFrontFace(GL_CW);

    FontResource *font;
    {
        DataFile df("core.data");
        if(df.isError()) {
            std::cerr << df.errorString() << "\n";
            return;
        }

        ResourceLoader rl(df, "resources.yaml");
        font = static_cast<FontResource*>(rl.load("core.font"));
    }

    ModelResource *ship1, *ship2, *ship3;
    {
        DataFile df("test.ship");
        if(df.isError()) {
            std::cerr << df.errorString() << "\n";
            return;
        }

        ResourceLoader rl(df, "resources.yaml");
        ship1 = static_cast<ModelResource*>(rl.load("ship.model1"));
        ship2 = static_cast<ModelResource*>(rl.load("ship.model2"));
        ship3 = static_cast<ModelResource*>(rl.load("ship.model3"));
    }

    glm::vec3 pos1;
    glm::vec3 vel1(0.01f, 0.005f, 0.0f);
    glm::vec3 pos2(-1.0f, 0.0f, 0.0f);
    glm::vec3 vel2(-0.01f, -0.005f, 0.0f);
    glm::vec3 pos3(1.0f, -1.0f, 0.0f);
    glm::vec3 vel3(0.015f, -0.005f, 0.0f);
    float r1 = 0, r2 = 0.5, r3 = 1.0;

    glm::mat4 proj = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    glm::mat4 view = glm::lookAt(
        glm::vec3(0,0,20),
        glm::vec3(0,0,0),
        glm::vec3(0,1,0)
    );

    glm::mat4 PV = proj * view;

    double lastTime = glfwGetTime();
    int frames=0, fps=0;
    string teststr = "Hello world!";
    do {
        glClear( GL_COLOR_BUFFER_BIT );

        // animate
        pos1 += vel1;
        pos2 += vel2;
        pos3 += vel3;
        r1 += 0.05;
        r2 += 0.05;
        r3 += 0.05;

        bounds(pos1, vel1);
        bounds(pos2, vel2);
        bounds(pos3, vel3);

        // draw
        glm::mat4 model = glm::rotate(glm::translate(glm::mat4(1.0f), pos1), r1, glm::vec3(0.0f, 0.0f, 1.0f));
        ship1->render(PV * model);

        glm::mat4 model2 = glm::rotate(glm::translate(glm::mat4(1.0f), pos2), r2, glm::vec3(0.0f, 0.0f, 1.0f));
        ship2->render(PV * model2);

        glm::mat4 model3 = glm::rotate(glm::translate(glm::mat4(1.0f), pos3), r3, glm::vec3(0.0f, 0.0f, 1.0f));
        ship3->render(PV * model3);

        font->text("FPS: %d", fps).scale(0.5).pos(1,1).align(TextRenderer::RIGHT).color(1,1,0).render();

        font->text(teststr).pos(-1,-0.8).color(0,1,0).render();

        glfwSwapBuffers();
        double lt = glfwGetTime();
        ++frames;
        if(lt - lastTime >= 0.1) {
            lastTime = lt;
            fps = frames * 10;
            frames = 0;
        }

    } while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS && glfwGetWindowParam( GLFW_OPENED ) );

    Resources::get().unloadResource("ship.model1");
    Resources::get().unloadResource("ship.model2");
    Resources::get().unloadResource("ship.model3");
}
