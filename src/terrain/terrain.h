#ifndef LUOLA_TERRAIN_H
#define LUOLA_TERRAIN_H

#include <GL/glfw.h>

#include "polygon.h"

namespace terrain {

/**
 * A terrain block.
 *
 * Terrain objects are collections of polygons that make up a logical terrain
 * block.
 *
 */
class Terrain {
public:
    Terrain() = delete;
    Terrain(const Terrain&) = delete;
    Terrain(const std::vector<ConvexPolygon> &polygons);
    ~Terrain();

    /**
     * Update OpenGL buffers.
     *
     * This must be called before calling draw() after the polygons have been
     * changed. Calling this function repeatedly does not incur a performance
     * penalty: if no changes have been made, the function will return immediately.
     */
    void updateGl();

    /**
     * Make a hole in the terrain.
     *
     * updateGl() must be called afterwards to update the graphics.
     * 
     * @param hole hole polygon
     * @return true if the terrain was changed
     */
    bool nibble(const ConvexPolygon &hole);
 
    /**
     * Draw the terrain
     *
     * @param transform transformation matrix
     */
    void draw(const glm::mat4 &transform) const;

private:
    void updateGl() const;

    std::vector<ConvexPolygon> m_polygons;
    bool m_dirty;
    GLuint m_vao;
    GLuint m_vbuffer;
    GLuint m_program;
    GLuint m_uniform_mvp;
    GLsizei m_gl_points;
};

}

#endif

