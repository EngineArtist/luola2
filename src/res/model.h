#ifndef LUOLA_RESOURCE_MODEL_H
#define LUOLA_RESOURCE_MODEL_H

#include <GL/glfw.h>
#include <glm/glm.hpp>

#include <vector>
#include <utility>

#include "resources.h"

using std::string;

namespace resource {

class Mesh;
class Program;
class Texture;

class Model : public Resource {
public:
    typedef std::pair<string, Texture*> SamplerTexture;
    typedef std::pair<GLuint, Texture*> UniformTexture;
    typedef std::vector<SamplerTexture> SamplerTextures;
    typedef std::vector<UniformTexture> UniformTextures;

    /**
     * Create a new model.
     *
     * A model is an abstraction of a vertex array object.
     * It holds vertex data (from the mesh), textures and a program.
     *
     * Model shaders have the following uniforms:
     * - MVP
     * - named texture samples (if any)
     *
     * Vertex attributes (in order):
     * 0: vertices
     * 1: normals
     * 3: UVs
     * 4: colors
     */
    static Model *make(
        const string& name,
        Mesh *mesh,
        Program *program,
        SamplerTextures textures
        );

    Model() = delete;
    ~Model();

    void render(const glm::mat4 &transform) const;

private:
    Model(const string& name, GLuint m_id, GLuint shader, GLuint mvp, GLsizei faces, const UniformTextures &textures);

    GLuint m_id;
    GLuint m_shader_id;
    GLuint m_mvp_id;
    GLsizei m_faces;
    UniformTextures m_textures;
};

}
#endif
