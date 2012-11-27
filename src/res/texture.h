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
#ifndef LUOLA_RESOURCE_TEXTURE_H
#define LUOLA_RESOURCE_TEXTURE_H

#include <GL/glfw.h>

#include "resources.h"

namespace fs { class DataFile; }

namespace resource {

class Texture : public Resource {
public:
    /**
     * Load a texture from a datafile.
     *
     * Currently only PNGs are supported.
     *
     * @param name resource name
     * @param datafile the datafile from which to load the texture
     * @param filename texture filename (inside datafile)
     * @return new Texture
     * @throw ResourceException in case of error
     */
    static Texture *load(const string& name, fs::DataFile &datafile, const string& filename);

    Texture() = delete;
    ~Texture();

    GLuint id() const { return m_id; }

    /**
     * Get the width of the texture
     *
     * @return image width
     */
    int width() const { return m_width; }

    /**
     * Get the height of the texture
     *
     * @return image height
     */
    int height() const { return m_height; }

    /**
     * Return the OpenGL texture target.
     *
     * This is GL_TEXTURE_2D for images.
     *
     * @return texture target
     */
    GLenum target() const { return m_target; }

private:
    Texture(const string& name, GLuint id, int width, int height, GLenum target);

    GLuint m_id;
    int m_width, m_height;
    GLenum m_target;
};

}

#endif
