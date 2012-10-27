#ifndef LUOLA_SHADER_RESOURCES_H
#define LUOLA_SHADER_RESOURCES_H

#include <GL/glfw.h>

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
     * @throw ResourceException in case of error
     */
    static ShaderResource *load(const string& name, DataFile &datafile, const string& filename, Type type);

    ShaderResource() = delete;
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
    ProgramResource() = delete;
    ~ProgramResource();

    /**
     * Create a new program.
     *
     * To finalize, you must add shaders to this program and finally call link().
     * The program will be automatically registered with the resource manager.
     *
     * @param name resource name
     */
    static ProgramResource *make(const string& name);

    /**
     * Add a shader to this program.
     *
     * This function cannot be called anymore after the program has been linked.
     * @param shader shader to add
     * @return reference to this
     */
    void addShader(ShaderResource *shader);

    /**
     * Link the program.
     * @throw ResourceException on error
     */
    void link();

    GLuint id() const { return m_id; }

private:
    ProgramResource(const string& name, GLuint id);

    GLuint m_id;
    bool m_linked;
};

#endif

