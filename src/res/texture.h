#ifndef LUOLA_RESOURCE_TEXTURE_H
#define LUOLA_RESOURCE_TEXTURE_H

#include <GL/gl.h>

#include "resources.h"

class DataFile;

class TextureResource : public Resource {
public:
    TextureResource() = delete;

    /**
     * Load a texture from a datafile.
     *
     * Currently only PNGs are supported.
     *
     * @param name resource name
     * @param datafile the datafile from which to load the texture
     * @param filename texture filename (inside datafile)
     * @return new TextureResource
     * @throw ResourceException in case of error
     */
    static TextureResource *load(const string& name, DataFile &datafile, const string& filename);

    ~TextureResource();

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

private:
    TextureResource(const string& name, GLuint id, int width, int height);

    GLuint m_id;
    int m_width, m_height;
};

#endif

