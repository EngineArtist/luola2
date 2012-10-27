#include <GL/glew.h>

#include "model.h"
#include "mesh.h"
#include "texture.h"

#include "shader.h"

ModelResource *ModelResource::make(
    const string& name,
    MeshResource *mesh,
    ProgramResource *program,
    SamplerTextures textures
    )
{
    // Create the vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Get uniform locations
    GLuint mvpid = glGetUniformLocation(program->id(), "MVP");

    UniformTextures utextures;
    for(const SamplerTexture &st : textures) {
        utextures.push_back(UniformTexture(
           glGetUniformLocation(program->id(), st.first.c_str()),
           st.second
           ));
    }

    // Set vertex data (0)
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferId());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Set normal data (1)
    if(mesh->normalBufferId()) {
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->normalBufferId());
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // Set UV data (2)
    if(mesh->uvBufferId()) {
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->uvBufferId());
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // TODO Set color data (3)

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Set element index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->elementArrayId());

    glBindVertexArray(0);

    ModelResource *res = new ModelResource(
        name,
        vao,
        program->id(),
        mvpid,
        mesh->faceCount(),
        utextures);

    Resources::get().registerResource(res);
    res->addDependency(mesh);
    res->addDependency(program);
    for(const SamplerTexture &st : textures)
        res->addDependency(st.second);

    return res;
}

ModelResource::ModelResource(const string& name, GLuint id, GLuint shader, GLuint mvp, GLsizei faces, const UniformTextures &textures)
    : Resource(name, MODEL), m_id(id), m_shader_id(shader),
      m_mvp_id(mvp), m_faces(faces), m_textures(textures)
{
}

ModelResource::~ModelResource()
{
    glDeleteVertexArrays(1, &m_id);
}

void ModelResource::render(const glm::mat4 &transform) const
{
    // Bind our vertex data
    glBindVertexArray(m_id);

    // Set shader program
    glUseProgram(m_shader_id);

    // Set uniforms
    glUniformMatrix4fv(m_mvp_id, 1, GL_FALSE, &transform[0][0]);

    // Set textures (if any)
    int texi = 0;
    for(const UniformTexture &t : m_textures) {
        glActiveTexture(GL_TEXTURE0 + texi);
        glBindTexture(t.second->target(), t.second->id());
        glUniform1i(t.first, texi);

        ++texi;
    }
 
    // Draw
    glDrawElements(GL_TRIANGLES, m_faces, GL_UNSIGNED_SHORT, 0);
}
