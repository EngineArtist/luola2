#include <iostream>

#include <GL/glew.h>

#include "shader.h"
#include "../fs/datafile.h"

ShaderResource *ShaderResource::load(const string& name, DataFile &datafile, const string &filename, Type type)
{
#ifndef NDEBUG
    std::cout << "Loading shader " << filename << "..." << std::endl;
#endif
    // Load shader source
    DataStream ds(datafile, filename);
    if(ds->isError()) {
        std::cerr << "Error loading shader " << filename << ": " << ds->errorString() << std::endl;
        return nullptr;
    }

    std::string program(
        (std::istreambuf_iterator<char>(ds)),
        std::istreambuf_iterator<char>()
        );
    if(ds->isError()) {
        std::cerr << "Error reading shader " << filename << ": " << ds->errorString() << std::endl;
        return nullptr;
    }

    // Compile Vertex Shader
    GLenum shaderType;
    switch(type) {
        case VERTEX_SHADER: shaderType = GL_VERTEX_SHADER; break;
        case FRAGMENT_SHADER: shaderType = GL_FRAGMENT_SHADER; break;
        default:
            std::cerr << "Unsupported shader type: " << type << "\n";
            return nullptr;
    }

    GLuint id = glCreateShader(shaderType);

    const char *shadersource = program.c_str();
    glShaderSource(id, 1, &shadersource, nullptr);
    glCompileShader(id);

    // Check compilation results
    GLint result = GL_FALSE;
    int infologlen;

    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(!result) {
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infologlen);
        std::vector<char> errormessage(infologlen);
        glGetShaderInfoLog(id, infologlen, nullptr, &errormessage[0]);

        std::cerr << "Error compiling shader: " << &errormessage[0];
        glDeleteShader(id);
        return nullptr;
    }

    // Create resource
    ShaderResource *res = new ShaderResource(name, type, id);
    Resources::get().registerResource(res);

    return res;
}

ShaderResource::ShaderResource(const string& name, Type type, GLuint id)
    : Resource(name, type), m_id(id)
{
}

ShaderResource::~ShaderResource()
{
    glDeleteShader(m_id);
}

ProgramResource *ProgramResource::make(const string& name)
{
    GLuint id = glCreateProgram();
    ProgramResource *res = new ProgramResource(name, id);
    Resources::get().registerResource(res);
    return res;
}

ProgramResource::ProgramResource(const string& name, GLuint id)
    : Resource(name, SHADER_PROGRAM), m_id(id)
{
}

void ProgramResource::addShader(ShaderResource *shader)
{
    assert(shader);
    if(shader) {
        glAttachShader(m_id, shader->id());
        addDependency(shader);
    }
}

bool ProgramResource::link()
{
#ifndef NDEBUG
    std::cout << "Linking shader program " << name() << "..." << std::endl;
#endif
    glLinkProgram(m_id);

    // Check the program
    GLint result = GL_FALSE;
    glGetProgramiv(m_id, GL_LINK_STATUS, &result);
    if(!result) {
        int infologlen;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infologlen);
        std::vector<char> errormessage(infologlen);
        glGetProgramInfoLog(m_id, infologlen, nullptr, &errormessage[0]);

        std::cerr << "Error linking program: " << errormessage[0] << "\n";
        return false;
    }
    return true;
}
