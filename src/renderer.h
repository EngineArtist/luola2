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
#ifndef LUOLA_RENDERER_H
#define LUOLA_RENDERER_H

class World;
namespace resource { class Font; }

class Renderer {
public:
    Renderer(const World &world);

    void render(double frametime) const;

private:
    const World &m_world;
    resource::Font *m_font;
};

#endif

