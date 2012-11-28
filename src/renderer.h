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
#ifndef LUOLA_RENDERER_H
#define LUOLA_RENDERER_H

#include <glm/glm.hpp>
#include "terrain/common.h"

class World;
namespace resource { class Font; }

class Renderer {
public:
    Renderer(const World &world);

    /**
     * Center the given point in the viewport.
     *
     * The viewport position is clamped so it never
     * goes outside the world bounds.
     *
     * @param point new viewpoint center
     */
    void setCenter(const terrain::Point &point);

    /**
     * Set the zoom factor.
     *
     * The zoom factor determines how many units will fit in the viewport.
     *
     * @param zoon zoom factor
     */
    void setZoom(float zoom);

    void render(double frametime) const;

private:
    void updateProjection();

    const World &m_world;

    terrain::Point m_center;
    float m_zoom;
    glm::mat4 m_projection;

    resource::Font *m_font;
};

#endif

