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

void Renderer::render(double frametime) const
{
    glClear( GL_COLOR_BUFFER_BIT );

    glm::mat4 proj = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    glm::mat4 view = glm::lookAt(
        glm::vec3(0,0,20),
                                 glm::vec3(0,0,0),
                                 glm::vec3(0,1,0)
    );

    glm::mat4 PV = proj * view; 
    for(const terrain::Zone *zone : m_world.m_zones)
        zone->draw(PV);

    for(const terrain::Solid *solid : m_world.m_static_terrain)
        solid->draw(PV);

    for(const terrain::Solid *solid : m_world.m_dyn_terrain)
        solid->draw(PV);

    for(const Ship &ship : m_world.m_ships)
        ship.draw(PV);

    Projectiles::getModel()->prepareRender();
    for(const Projectile &p : m_world.m_projectiles)
        p.draw(PV);
    Projectiles::getModel()->endRender();

    m_font->text("FPS: %.1f", 1.0 / frametime)
    .scale(0.5).pos(1,1).align(resource::TextRenderer::RIGHT).color(1,1,0)
    .render();

    glfwSwapBuffers();
}
