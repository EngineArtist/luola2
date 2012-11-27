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
#ifndef LUOLA_SHADER_RESOURCES_H
#define LUOLA_SHADER_RESOURCES_H

#include <GL/glfw.h>

#include "resources.h"

namespace fs { class DataFile; }

namespace resource {

class Shader : public Resource {
public:
    /**
     * Load a shader from a datafile.
     *
     * The resource will automatically be registered with the resource manager.
     *
     * @param name resource name
     * @param datafile the datafile from which to load the shader
     * @param filename shader file name (inside datafile)
     * @param type resource type. Must be one of the _SHADER types.
     * @return new Shader
     * @throw ResourceException in case of error
     */
    static Shader *load(const string& name, fs::DataFile &datafile, const string& filename, Type type);

    Shader() = delete;
    ~Shader();

    GLuint id() const { return m_id; }

private:
    Shader(const string& name, Type type, GLuint id);

    GLuint m_id;
};

/**
 * A shader program
 */
class Program : public Resource {
public:
    Program() = delete;
    ~Program();

    /**
     * Create a new program.
     *
     * To finalize, you must add shaders to this program and finally call link().
     * The program will be automatically registered with the resource manager.
     *
     * @param name resource name
     */
    static Program *make(const string& name);

    /**
     * Add a shader to this program.
     *
     * This function cannot be called anymore after the program has been linked.
     * @param shader shader to add
     * @return reference to this
     */
    void addShader(Shader *shader);

    /**
     * Link the program.
     * @throw ResourceException on error
     */
    void link();

    GLuint id() const { return m_id; }

private:
    Program(const string& name, GLuint id);

    GLuint m_id;
    bool m_linked;
};

}

#endif
