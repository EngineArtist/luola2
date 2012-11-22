#include <iostream>
#include <GL/glew.h>

#include "terrain.h"
#include "../res/shader.h"

namespace terrain {

Terrain::Terrain(const std::vector<ConvexPolygon> &polygons)
    : m_polygons(polygons), m_dirty(true)
{

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbuffer);

    // TODO set these properly
    m_program = resource::get<resource::Program>("core.shader.terrain")->id();
    m_uniform_mvp = glGetUniformLocation(m_program, "MVP");
}

Terrain::~Terrain()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbuffer);
}

bool Terrain::hasPoint(const Point &p) const
{
    for(const ConvexPolygon &poly : m_polygons) {
        if(poly.hasPoint(p))
            return true;
    }
    return false;
}

bool Terrain::circleCollision(const Point &p, float r, const glm::vec2 &v, Point &cp, glm::vec2 &normal) const
{
    for(const ConvexPolygon &poly : m_polygons) {
        if(poly.circleCollision(p, r, v, cp, normal))
            return true;
    }
    return false;
}

bool Terrain::nibble(const ConvexPolygon &hole)
{
    // First, check that there are overlaps in the first place
    int found = -1;
    for(unsigned int i=0;i<m_polygons.size();++i) {
        if(m_polygons[i].overlaps(hole)) {
            found = i;
            break;
        }
    }

    if(found>=0) {
        // If overlapping polygons were found, generate a new list.
        // A single polygon can be removed altogether or may be split
        // into multiple smaller polygons.

        std::vector<ConvexPolygon> newpolys;

        // We already know that polygons [0..found) don't overlap
        for(int i=0;i<found;++i)
            newpolys.push_back(std::move(m_polygons[i]));

        // First overlap
        m_polygons[found].booleanDifference(hole, newpolys);

        // Check the reset
        for(unsigned int i=found+1;i<m_polygons.size();++i) {
            ConvexPolygon &p = m_polygons[i];
            if(p.overlaps(hole))
                p.booleanDifference(hole, newpolys);
            else
                newpolys.push_back(std::move(p));
        }
        //newpolys.push_back(hole);
        m_polygons = std::move(newpolys);
        m_dirty = true;
    }

    return found>=0;
}

void Terrain::draw(const glm::mat4 &transform) const
{
    glBindVertexArray(m_vao);
    glUseProgram(m_program);
    glUniformMatrix4fv(m_uniform_mvp, 1, GL_FALSE, &transform[0][0]);

    glDrawArrays(GL_LINES, 0, m_gl_points);
    glBindVertexArray(0);
}

void Terrain::updateGl()
{
    if(!m_dirty)
        return;

    // Gather terrain triangles
    std::vector<Point> points;
    for(const ConvexPolygon &poly : m_polygons)
        poly.toTriangles(points);
    m_gl_points = points.size();

    glBindVertexArray(m_vao);

    // Vertex data
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbuffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(Point) * m_gl_points,
        points.data(),
        GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_dirty = false;
}

}
