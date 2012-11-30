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
#include <iostream>
#include <GL/glew.h>
#include <glm/gtx/norm.hpp>

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
    float nearest = 9999.0f;

    Point colp;
    glm::vec2 colnormal;
    for(const ConvexPolygon &poly : m_polygons) {
        if(poly.circleCollision(p, r, v, colp, colnormal)) {
            float dist = glm::distance2(colp, p);
            if(dist < nearest) {
                nearest = dist;
                cp = colp;
                normal = colnormal;
            }
        }
    }
    return nearest < 9999.0f;
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
