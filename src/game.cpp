#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>

#include "fs/datafile.h"
#include "res/shader.h"
#include "game.h"

void gameloop()
{
    glfwEnable( GLFW_STICKY_KEYS );

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    {
        DataFile df("test.shaders");
        if(df.isError()) {
            std::cerr << df.errorString() << "\n";
            return;
        }

        if(!(ProgramResource::make("program")
            (ShaderResource::load("vertex", df, "vertex.shader", Resource::VERTEX_SHADER))
            (ShaderResource::load("fragment", df, "fragment.shader", Resource::FRAGMENT_SHADER))
            .link()))
            return;
    }

    ProgramResource *program = static_cast<ProgramResource*>(Resources::get().getResource("program"));

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
    };

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    do {
        glClear( GL_COLOR_BUFFER_BIT );

        glUseProgram(program->id());

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

        glDrawArrays(GL_TRIANGLES, 0, 3); // From index 0 to 3 -> 1 triangle

        glDisableVertexAttribArray(0);

        glfwSwapBuffers();

    } while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS && glfwGetWindowParam( GLFW_OPENED ) );

    Resources::get().unloadResource("program");

    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
}
