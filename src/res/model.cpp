#include <GL/glew.h>

#include "model.h"
#include "mesh.h"
#include "texture.h"

#include "shader.h"

namespace resource {

Model *Model::make(
    const string& name,
    Mesh *mesh,
    Program *program,
    SamplerTextures textures,
    bool blend
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

    Model *res = new Model(
        name,
        mesh,
        vao,
        program->id(),
        mvpid,
        utextures,
        blend
        );

    Resources::getInstance().registerResource(res);
    res->addDependency(mesh);
    res->addDependency(program);
    for(const SamplerTexture &st : textures)
        res->addDependency(st.second);

    return res;
}

Model::Model(const string& name, const Mesh *mesh, GLuint id, GLuint shader, GLuint mvp, const UniformTextures &textures, bool blend)
    : Resource(name, MODEL), m_id(id), m_shader_id(shader),
      m_mvp_id(mvp), m_textures(textures), m_blend(blend), m_mesh(mesh)
{
}

Model::~Model()
{
    glDeleteVertexArrays(1, &m_id);
}

void Model::prepareRender() const
{
    // Bind our vertex data
    glBindVertexArray(m_id);

    // Set shader program
    glUseProgram(m_shader_id);

    // Set textures (if any)
    int texi = 0;
    for(const UniformTexture &t : m_textures) {
        glActiveTexture(GL_TEXTURE0 + texi);
        glBindTexture(t.second->target(), t.second->id());
        glUniform1i(t.first, texi);
        ++texi;
    }

    // Blending
    if(m_blend) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void Model::endRender() const
{
    glBindVertexArray(0);
    glUseProgram(0);
    glDisable(GL_BLEND);
}

void Model::render(const glm::mat4 &transform) const
{
    glUniformMatrix4fv(m_mvp_id, 1, GL_FALSE, &transform[0][0]);
    glDrawElements(GL_TRIANGLES, m_mesh->faceCount(), GL_UNSIGNED_SHORT, 0);
}

void Model::render(const glm::mat4 &transform, const string &name) const
{
    MeshSlice offset = m_mesh->submeshOffset(name);
    render(transform, offset.first, offset.second);
}

void Model::render(const glm::mat4 &transform, GLushort offset, GLsizei len) const
{
    glUniformMatrix4fv(m_mvp_id, 1, GL_FALSE, &transform[0][0]);
    glDrawElements(GL_TRIANGLES, len, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid*>(sizeof(GLushort) * offset));
}

}
