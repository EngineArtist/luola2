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
    explicit Terrain(const std::vector<ConvexPolygon> &polygons);
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

    /**
     * Check if the given point is inside this terrain block.
     *
     * @param p the point to check
     * @return true if the point is in any of the subpolygons of this block
     */
    bool hasPoint(const Point &p) const;

    /**
     * Check for a collision with a moving circle.
     *
     * If a collision is detected, true is returned
     * and the parameter cp and normal will be set.
     *
     * @param p circle center point
     * @param r circle radius
     * @param v circle displacement in this timestep
     * @param cp circle's center point at collision
     * @param normal normal of the colliding edge
     * @return true if collision happens
     */
    bool circleCollision(const Point &p, float r, const glm::vec2 &v, Point &cp, glm::vec2 &normal) const;

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

