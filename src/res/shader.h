#ifndef LUOLA_SHADER_RESOURCES_H
#define LUOLA_SHADER_RESOURCES_H

#include <GL/gl.h>

#include "resources.h"

class DataFile;

class ShaderResource : public Resource {
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
     * @return new ShaderResource
     */
    static ShaderResource *load(const string& name, DataFile &datafile, const string& filename, Type type);

    ~ShaderResource();

    GLuint id() const { return m_id; }

private:
    ShaderResource(const string& name, Type type, GLuint id);

    GLuint m_id;
};

/**
 * A shader program
 */
class ProgramResource : public Resource {
public:
    /**
     * Create a new program.
     * To finalize, you must add shaders to this program and finally call link().
     */
    static ProgramResource *make(const string& name);

    /**
     * Add a shader to this program.
     * @param shader shader to add
     * @return reference to this
     */
    void addShader(ShaderResource *shader);

    /**
     * Link the program.
     * @return true on success
     */
    bool link();

    GLuint id() const { return m_id; }

private:
    ProgramResource(const string& name, GLuint id);

    GLuint m_id;
};

#endif

