//
// This file is part of Luola2.
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
#include <glm/gtc/matrix_transform.hpp>

#include "world.h"
#include "renderer.h"
#include "res/font.h"
#include "res/model.h"

#include "ship/ship.h"
#include "projectile/projectiledef.h"

Renderer::Renderer(const World &world)
    : m_world(world)
{
    m_font = resource::get<resource::Font>("core.font.default");
}

void Renderer::setCenter(const terrain::Point &point)
{
    m_center = point;
    updateProjection();
}

void Renderer::setZoom(float zoom)
{
    assert(zoom > 0.0f);
    m_zoom = zoom;
    updateProjection();
}

void Renderer::updateProjection()
{
    glm::mat4 proj = glm::ortho(-m_zoom, m_zoom, -m_zoom, m_zoom);
    // TODO prevent viewport from going outside world bounds
    m_projection = glm::translate(proj, -glm::vec3(m_center, 0));
}

void Renderer::render(double frametime) const
{
    glClear( GL_COLOR_BUFFER_BIT );

    for(const terrain::Zone *zone : m_world.m_zones)
        zone->draw(m_projection);

    for(const terrain::Solid *solid : m_world.m_static_terrain)
        solid->draw(m_projection);

    for(const terrain::Solid *solid : m_world.m_dyn_terrain)
        solid->draw(m_projection);

    for(const Ship &ship : m_world.m_ships)
        ship.draw(m_projection);

    Projectiles::getModel()->prepareRender();
    for(const Projectile &p : m_world.m_projectiles)
        p.draw(m_projection);
    Projectiles::getModel()->endRender();

    m_font->text("FPS: %.1f", 1.0 / frametime)
    .scale(0.5).pos(1,1).align(resource::TextRenderer::RIGHT).color(1,1,0)
    .render();

    glfwSwapBuffers();
}
