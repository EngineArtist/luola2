#ifndef LUOLA_RESOURCE_MESH_H
#define LUOLA_RESOURCE_MESH_H

#include <GL/glfw.h>

#include "resources.h"

class DataFile;

class MeshResource : public Resource {
public:
    /**
     * Load a 3D model from a datafile.
     *
     * The model is in a simple ASCII format.
     * The first line is header line, with the following syntax:
     * count "vertices" [N] [UV] [C]
     *
     * The first column (count) is the number of vertices to follow.
     * The second column is the name of the header.
     * The following columns are optional but must appear in the above order.
     * Their presence indicates the presence of the respective data columns as well.
     * N means vertex normals are included. (3 columns)
     * UV means UV coordinates are included. (2 columns)
     * C means color values (RGB) are included. (3 columns)
     *
     * The next (count) lines are vertex definitions. The first three columns
     * are the X, Y and Z coordinates. The presence of additional columns
     * is indicated by the N, UV and C flags.
     *
     * After vertex data, triangles are defined. Triangle header has the following
     * syntax:
     * count "triangles"
     *
     * The next (count) lines have three columns with integers indexing the
     * vertex definitions.
     *
     * @param name resource name
     * @param datafile the datafile from which to load the model
     * @param filename model file name (inside datafile)
     * @return new MeshResource
     * @throw ResourceException in case of error
     */
    static MeshResource *load(const string& name, DataFile &datafile, const string& filename);

    MeshResource() = delete;
    ~MeshResource();

    GLuint vertexBufferId() const { return m_vertex; }

    GLuint normalBufferId() const { return m_normal; }

    GLuint uvBufferId() const { return m_uv; }

    GLuint colorBufferId() const { return m_color; }

    GLuint elementArrayId() const { return m_element; }

    /**
     * Get the number of vertices in this mesh.
     *
     * @return vertex count
     */
    int vertexCount() const { return m_vertices; }

    /**
     * Get the number of faces (triangles) in this mesh.
     *
     * @return face count
     */
    int faceCount() const { return m_faces; }

private:

    MeshResource(const string& name, GLuint vertexid, GLuint elementid, GLuint normalid, GLuint uvid, GLuint colorid, int vertices, int faces);

    GLuint m_vertex;
    GLuint m_element;
    GLuint m_normal;
    GLuint m_uv;
    GLuint m_color;
    GLuint m_elements;
    int m_vertices;
    int m_faces;
};

#endif

