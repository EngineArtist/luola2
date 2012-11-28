//
// This file is part of Luola2.
// Copyright (C) 2012 Calle Laakkonen
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
#ifndef NDEBUG
#include <iostream>
using std::cerr;
using std::endl;
#endif

#include <GL/glew.h>

#include "shader.h"
#include "../fs/datafile.h"

namespace resource {

Shader *Shader::load(
    const string& name,
    fs::DataFile &datafile,
    const string &filename,
    Type type)
{
#ifndef NDEBUG
    cerr << "Loading shader " << filename << "..." << endl;
#endif
    // Load shader source
    fs::DataStream ds(datafile, filename);
    if(ds->isError())
        throw ResourceException(datafile.name(), name, ds->errorString());

    std::string program(
        (std::istreambuf_iterator<char>(ds)),
        std::istreambuf_iterator<char>()
        );
    if(ds->isError())
        throw ResourceException(datafile.name(), name, ds->errorString());

    // Compile Vertex Shader
    GLenum shaderType;
    switch(type) {
        case VERTEX_SHADER: shaderType = GL_VERTEX_SHADER; break;
        case GEOMETRY_SHADER: shaderType = GL_GEOMETRY_SHADER; break;
        case FRAGMENT_SHADER: shaderType = GL_FRAGMENT_SHADER; break;
        default:
            throw ResourceException(datafile.name(), name, "Unsupported shader type");
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
        glDeleteShader(id);

        throw ResourceException(datafile.name(), name, &errormessage[0]);
    }

    // Create resource
    Shader *res = new Shader(name, type, id);
    Resources::getInstance().registerResource(res);

    return res;
}

Shader::Shader(const string& name, Type type, GLuint id)
    : Resource(name, type), m_id(id)
{
}

Shader::~Shader()
{
    glDeleteShader(m_id);
}

Program *Program::make(const string& name)
{
    GLuint id = glCreateProgram();
    Program *res = new Program(name, id);
    Resources::getInstance().registerResource(res);
    return res;
}

Program::Program(const string& name, GLuint id)
    : Resource(name, SHADER_PROGRAM), m_id(id), m_linked(false)
{
}

Program::~Program()
{
    glDeleteProgram(m_id);
}

void Program::addShader(Shader *shader)
{
    if(m_linked)
        throw ResourceException("", name(), "tried to add shader to a linked program");
    if(!shader)
        throw ResourceException("", name(), "tried to add null shader to this program");

    glAttachShader(m_id, shader->id());
    addDependency(shader);
}

void Program::link()
{
#ifndef NDEBUG
    cerr << "Linking shader program " << name() << "..." << endl;
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

        throw ResourceException("", name(), &errormessage[0]);
    }
    m_linked = true;
}

}
