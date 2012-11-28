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

#include <boost/algorithm/string.hpp>
#include <vector>
#include <cstdlib>

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/glfw.h>

//#include <GL/glew.h>

#include "mesh.h"
#include "../fs/datafile.h"

namespace resource {

namespace {
struct MeshData {
    std::vector<glm::vec3> vertex;
    std::vector<glm::vec3> normal;
    std::vector<glm::vec2> uv;
    std::vector<GLushort> triangle;
};

void loadMesh(fs::DataFile &datafile, const string &filename, MeshData &data)
    {    fs::DataStream ds(datafile, filename);
    if(ds->isError())
        throw ResourceException(datafile.name(), filename, ds->errorString());

    enum {
        EXPECT_VERTEX_HEADER,
        EXPECT_VERTEX,
        EXPECT_TRIANGLE_HEADER,
        EXPECT_TRIANGLE,
        DONE
    } state = EXPECT_VERTEX_HEADER;

    int expecting = 0;
    bool has_normal = false;
    bool has_uv = false;
    bool has_color = false;

    const int base = data.vertex.size();
 
    string line;
    while(state != DONE) {
        std::getline(ds, line);
        if(ds.eof())
            break;

        std::vector<string> tokens;
        boost::split(tokens, line, boost::is_any_of(" \t"), boost::token_compress_on);

        switch(state) {
            case EXPECT_VERTEX_HEADER:
                if(tokens.size() < 2 || tokens.size() > 5)
                    throw ResourceException(datafile.name(), filename,
                                            "Expected 2-5 tokens on mesh header!");

                    if(tokens[1] != "vertices")
                        throw ResourceException(datafile.name(), filename,
                                                "First token of mesh file is not \"vertices\"!");

                        expecting = atoi(tokens[0].c_str());
                    for(unsigned int t=2;t<tokens.size();++t) {
                        if(tokens[t] == "N")
                            has_normal = true;
                        else if(tokens[t] == "UV")
                            has_uv = true;
                        else if(tokens[t] == "C")
                            has_color = true;
                        else
                            throw ResourceException(datafile.name(), filename,
                                                    "First vertex header flag: " + tokens[t]);
                    }
                   state = EXPECT_VERTEX;
                    break;

            case EXPECT_VERTEX:
                data.vertex.push_back(glm::vec3(
                    atof(tokens.at(0).c_str()),
                    atof(tokens.at(1).c_str()),
                    atof(tokens.at(2).c_str())
                    ));

                {
                    int i=3;
                    if(has_normal) {
                        data.normal.push_back(glm::vec3(
                            atof(tokens.at(i+0).c_str()),
                                                   atof(tokens.at(i+1).c_str()),
                                                   atof(tokens.at(i+2).c_str())
                        ));
                        i += 3;
                    }
                    if(has_uv) {
                        data.uv.push_back(glm::vec2(
                            atof(tokens.at(i+0).c_str()),
                                               atof(tokens.at(i+1).c_str())
                        ));
                        i += 2;
                    }
                    // TODO color
                }

                if(--expecting == 0)
                    state = EXPECT_TRIANGLE_HEADER;
                break;

            case EXPECT_TRIANGLE_HEADER:
                if(tokens.at(1) != "triangles")
                    throw ResourceException(datafile.name(), filename,
                                            "Expected triangle header!");

                    expecting = atoi(tokens.at(0).c_str());
                state = EXPECT_TRIANGLE;
                break;

            case EXPECT_TRIANGLE:
                data.triangle.push_back(base + atoi(tokens.at(0).c_str()));
                data.triangle.push_back(base + atoi(tokens.at(1).c_str()));
                data.triangle.push_back(base + atoi(tokens.at(2).c_str()));

                if(--expecting == 0)
                    state = DONE;
                break;

            case DONE: break;
        }
    }
}
}

Mesh *Mesh::load(
    const string& name,
    fs::DataFile &datafile,
    const std::unordered_map<string, string> &filenames,
    const glm::vec3 &offset,
    const glm::vec3 &scale
    )
{
    MeshData data;
    std::unordered_map<string, MeshSlice> submeshes;

    // Load all meshes
    for(const auto &submesh : filenames) {
#ifndef NDEBUG
        cerr << "Loading mesh " << submesh.second << " (" << name << ")..." << endl;
#endif

        int off = data.triangle.size();
        loadMesh(datafile, submesh.second, data);
        cerr << "submesh " << off << ":" << data.triangle.size() << endl;
        submeshes[submesh.first] = MeshSlice(off, data.triangle.size()-off);
    }

    // Apply offset and scale
    for(glm::vec3 &vec : data.vertex)
        vec = (vec + offset) * scale;

    // Create vertex, normal, color and UV buffers
    GLuint vbId;
    glGenBuffers(1, &vbId);
    glBindBuffer(GL_ARRAY_BUFFER, vbId);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(glm::vec3) * data.vertex.size(),
        data.vertex.data(),
        GL_STATIC_DRAW);

    GLuint indId;
    glGenBuffers(1, &indId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indId);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLushort) * data.triangle.size(),
        data.triangle.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLuint nbId = 0;
    if(!data.normal.empty()) {
        glGenBuffers(1, &nbId);
        glBindBuffer(GL_ARRAY_BUFFER, nbId);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(glm::vec3) * data.normal.size(),
            data.normal.data(),
            GL_STATIC_DRAW);
    }

    GLuint uvId = 0;
    if(!data.uv.empty()) {
        glGenBuffers(1, &uvId);
        glBindBuffer(GL_ARRAY_BUFFER, uvId);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(glm::vec2) * data.uv.size(),
            data.uv.data(),
            GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Mesh *res = new Mesh(
        name,
        vbId,
        indId,
        nbId,
        uvId,
        0,
        data.vertex.size(),
        data.triangle.size(),
        submeshes
        );

    Resources::getInstance().registerResource(res);
    return res;
}

Mesh::Mesh(const string& name, GLuint vertexid, GLuint elementid, GLuint normalid, GLuint uvid, GLuint colorid, int vertices, int faces, const std::unordered_map<string, MeshSlice> &submeshes)
    : Resource(name, MESH),
      m_vertex(vertexid), m_element(elementid), m_normal(normalid), m_uv(uvid),
      m_color(colorid), m_vertices(vertices), m_faces(faces), m_meshes(submeshes)
{
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_vertex);
    glDeleteBuffers(1, &m_element);
}

}
