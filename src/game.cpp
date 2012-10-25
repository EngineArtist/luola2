#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "fs/datafile.h"
#include "res/shader.h"
#include "res/texture.h"
#include "res/mesh.h"
#include "res/loader.h"
#include "game.h"

void gameloop()
{
    glfwEnable( GLFW_STICKY_KEYS );

    ProgramResource *program;
    TextureResource *texture;
    MeshResource *mesh;
    {
        DataFile df("test.ship");
        if(df.isError()) {
            std::cerr << df.errorString() << "\n";
            return;
        }

        ResourceLoader rl(df, "resources.json");
        mesh = static_cast<MeshResource*>(rl.load("ship:mesh"));
        texture = static_cast<TextureResource*>(rl.load("ship:texture"));
        program = static_cast<ProgramResource*>(rl.load("ship:shader"));
    }

    GLuint TextureID  = glGetUniformLocation(program->id(), "myTextureSampler");
    GLuint MatrixID = glGetUniformLocation(program->id(), "MVP");

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id());
    glUniform1i(TextureID, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferId());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->uvBufferId());
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->elementArrayId());

    float x=0, y=0, vx=0.01, vy=0.005, r=0.0;

    glm::mat4 proj = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 view = glm::lookAt(
        glm::vec3(0,0,20),
        glm::vec3(0,0,0),
        glm::vec3(0,1,0)
    );
    glFrontFace(GL_CW);

    do {
        glClear( GL_COLOR_BUFFER_BIT );

        // animate
        x += vx;
        y += vy;
        r += 0.05;
        if(x<-10 || x>10)
            vx = -vx;
        if(y<-7.5 || y>7.5)
            vy = -vy;

        glm::mat4 model = glm::rotate(glm::translate(glm::mat4(1.0f),glm::vec3(x, y,0)), r, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 MVP = proj * view * model;

        // draw
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUseProgram(program->id());
        //glDrawArrays(GL_TRIANGLES, 0, 6); // From index 0 to 3 -> 1 triangle
        glDrawElements(GL_TRIANGLES, mesh->faceCount(), GL_UNSIGNED_SHORT, 0);

        glfwSwapBuffers();

    } while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS && glfwGetWindowParam( GLFW_OPENED ) );

    Resources::get().unloadResource("program");

    glDeleteVertexArrays(1, &VertexArrayID);
}
