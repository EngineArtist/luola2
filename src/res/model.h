//
// This file is part of Luola2.
//
// Luola2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Luola2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Luola2.  If not, see <http://www.gnu.org/licenses/>.
//
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
        SamplerTextures textures,
        bool blend
        );

    Model() = delete;
    ~Model();

    /**
     * Get the Mesh of this model.
     *
     * @return mesh
     */
    const Mesh *mesh() const { return m_mesh; }

    /**
     * Bind vertex arrays, textures and the shader program.
     *
     * After this call, render() can be called repeatedly to render this model.
     */
    void prepareRender() const;

    /**
     * Render the entire mesh.
     *
     * Use this when rendering a mesh with no submeshes.
     *
     * @param transform the transformation to apply
     */
    void render(const glm::mat4 &transform) const;

    /**
     * Render the named submesh.
     *
     * @param transform the transformation to apply
     * @param name the name of the submesh to render
     */
    void render(const glm::mat4 &transform, const string &name) const;

    /**
     * Render the submesh.
     *
     * @param transform the transformation to apply
     * @param offset vertex element array offset
     * @param len number of vertex elements to draw
     */
    void render(const glm::mat4 &transform, GLushort offset, GLsizei len) const;

    /**
     * Clean up after rendering
     */
    void endRender() const;

private:
    Model(const string& name, const Mesh *mesh, GLuint m_id, GLuint shader, GLuint mvp, const UniformTextures &textures, bool blend);

    GLuint m_id;
    GLuint m_shader_id;
    GLuint m_mvp_id;
    UniformTextures m_textures;

    bool m_blend;

    const Mesh *m_mesh;
};

}
#endif
