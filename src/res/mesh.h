#ifndef LUOLA_RESOURCE_MESH_H
#define LUOLA_RESOURCE_MESH_H

#include <GL/glfw.h>
#include <glm/glm.hpp>

#include <unordered_map>

#include "resources.h"

namespace fs { class DataFile; }

namespace resource {

typedef std::pair<int, int> MeshSlice;

class Mesh : public Resource {
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
     * Multiple meshes can be loaded into the same vertex array. The offsets
     * and element array lengths can be queried with submeshOffset().
     * 
     * @param name resource name
     * @param datafile the datafile from which to load the model
     * @param filenames map of mesh names to model file names (inside datafile)
     * @param offset offset to apply to each vertex
     * @param scale scaling factor to apply to each vertex. Scaling is done after offsetting
     * @return new Mesh
     * @throw ResourceException in case of error
     */
    static Mesh *load(
        const string& name,
        fs::DataFile &datafile,
        const std::unordered_map<string, string> &filenames,
        const glm::vec3 &offset,
        const glm::vec3 &scale
        );

    Mesh() = delete;
    ~Mesh();

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

    /**
     * Get the offset to and the length of the sub-mesh element array.
     *
     * If there are no named submeshes, the default mesh will be available
     * with the name "0".
     * 
     * @param name mesh name
     * @return mesh offset and vertex count
     */
     MeshSlice submeshOffset(const string &name) const { return m_meshes.at(name); }
 
private:

    Mesh(const string& name, GLuint vertexid, GLuint elementid, GLuint normalid, GLuint uvid, GLuint colorid, int vertices, int faces, const std::unordered_map<string, MeshSlice> &submeshes);

    GLuint m_vertex;
    GLuint m_element;
    GLuint m_normal;
    GLuint m_uv;
    GLuint m_color;
    GLuint m_elements;
    int m_vertices;
    int m_faces;

    std::unordered_map<string, MeshSlice> m_meshes;
};

}

#endif

