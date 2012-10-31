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

using std::string;

MeshResource *MeshResource::load(
    const string& name,
    DataFile &datafile,
    const string &filename,
    const glm::vec3 &offset,
    const glm::vec3 &scale
    )
{
#ifndef NDEBUG
    cerr << "Loading mesh " << filename << "..." << endl;
#endif

    DataStream ds(datafile, filename);
    if(ds->isError())
        throw ResourceException(datafile.name(), name, ds->errorString());

    enum {
        EXPECT_VERTEX_HEADER,
        EXPECT_VERTEX,
        EXPECT_TRIANGLE_HEADER,
        EXPECT_TRIANGLE,
        DONE
        } state ;
    int expecting = 0;
    bool has_normal = false;
    bool has_uv = false;
    bool has_color = false;

    std::vector<glm::vec3> vertex;
    std::vector<glm::vec3> normal;
    std::vector<glm::vec2> uv;
    std::vector<GLushort> triangle;

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
                    throw ResourceException(datafile.name(), name,
                        "Expected 2-5 tokens on mesh header!");

                if(tokens[1] != "vertices")
                    throw ResourceException(datafile.name(), name,
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
                        throw ResourceException(datafile.name(), name,
                            "First vertex header flag: " + tokens[t]);
                }
                vertex.reserve(expecting);
                state = EXPECT_VERTEX;
                break;

            case EXPECT_VERTEX:
                vertex.push_back((glm::vec3(
                    atof(tokens.at(0).c_str()),
                    atof(tokens.at(1).c_str()),
                    atof(tokens.at(2).c_str())
                    ) + offset) * scale);
                {
                    int i=3;
                    if(has_normal) {
                        normal.push_back(glm::vec3(
                            atof(tokens.at(i+0).c_str()),
                            atof(tokens.at(i+1).c_str()),
                            atof(tokens.at(i+2).c_str())
                            ));
                        i += 3;
                    }
                    if(has_uv) {
                        uv.push_back(glm::vec2(
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
                    throw ResourceException(datafile.name(), name,
                        "Expected triangle header!");

                expecting = atoi(tokens.at(0).c_str());
                triangle.reserve(expecting);
                state = EXPECT_TRIANGLE;
                break;

            case EXPECT_TRIANGLE:
                triangle.push_back(atoi(tokens.at(0).c_str()));
                triangle.push_back(atoi(tokens.at(1).c_str()));
                triangle.push_back(atoi(tokens.at(2).c_str()));

                if(--expecting == 0)
                    state = DONE;
                break;

            case DONE: break;
        }
    }

    GLuint vbId;
    glGenBuffers(1, &vbId);
    glBindBuffer(GL_ARRAY_BUFFER, vbId);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(glm::vec3) * vertex.size(),
        vertex.data(),
        GL_STATIC_DRAW);

    GLuint indId;
    glGenBuffers(1, &indId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indId);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLushort) * triangle.size(),
        triangle.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLuint nbId = 0;
    if(has_normal) {
        glGenBuffers(1, &nbId);
        glBindBuffer(GL_ARRAY_BUFFER, nbId);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(glm::vec3) * normal.size(),
            normal.data(),
            GL_STATIC_DRAW);
    }

    GLuint uvId = 0;
    if(has_uv) {
        glGenBuffers(1, &uvId);
        glBindBuffer(GL_ARRAY_BUFFER, uvId);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(glm::vec2) * uv.size(),
            uv.data(),
            GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    MeshResource *res = new MeshResource(
        name,
        vbId,
        indId,
        nbId,
        uvId,
        0,
        vertex.size(),
        triangle.size()
        );
    Resources::getInstance().registerResource(res);
    return res;
}

MeshResource::MeshResource(const string& name, GLuint vertexid, GLuint elementid, GLuint normalid, GLuint uvid, GLuint colorid, int vertices, int faces)
    : Resource(name, MESH),
      m_vertex(vertexid), m_element(elementid), m_normal(normalid), m_uv(uvid),
      m_color(colorid), m_vertices(vertices), m_faces(faces)
{
}

MeshResource::~MeshResource()
{
    glDeleteBuffers(1, &m_vertex);
    glDeleteBuffers(1, &m_element);
}
