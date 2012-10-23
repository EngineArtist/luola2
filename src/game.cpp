#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "fs/datafile.h"
#include "res/shader.h"
#include "res/texture.h"
#include "res/loader.h"
#include "game.h"

void gameloop()
{
    glfwEnable( GLFW_STICKY_KEYS );

    ProgramResource *program;
    TextureResource *texture;
    {
        DataFile df("test.shaders");
        if(df.isError()) {
            std::cerr << df.errorString() << "\n";
            return;
        }

        ResourceLoader rl(df, "sample.res");
        program = static_cast<ProgramResource*>(rl.load("program"));

        texture = static_cast<TextureResource*>(rl.load("texture"));
    }


    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
    };

    static const GLfloat g_uv_buffer_data[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f,
    };

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

    GLuint TextureID  = glGetUniformLocation(program->id(), "myTextureSampler");

    GLuint MatrixID = glGetUniformLocation(program->id(), "MVP");

    float x=0, y=0, vx=0.02, vy=0.01, r;

    glm::mat4 proj = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 view = glm::lookAt(
        glm::vec3(0,0,20),
        glm::vec3(0,0,0),
        glm::vec3(0,1,0)
    );

    do {
        glClear( GL_COLOR_BUFFER_BIT );

        // animate
        x += vx;
        y += vy;
        r += 0.1;
        if(x<-10 || x>10)
            vx = -vx;
        if(y<-7.5 || y>7.5)
            vy = -vy;

        // draw
        glm::mat4 model = glm::rotate(glm::translate(glm::mat4(1.0f),glm::vec3(x, y,0)), r, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 MVP = proj * view * model;

        glUseProgram(program->id());

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->id());
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(TextureID, 0);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*)0
        );

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
            1,
            2,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*)0
        );

        glDrawArrays(GL_TRIANGLES, 0, 3); // From index 0 to 3 -> 1 triangle

        glDisableVertexAttribArray(0);

        glfwSwapBuffers();

    } while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS && glfwGetWindowParam( GLFW_OPENED ) );

    Resources::get().unloadResource("program");

    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
}
