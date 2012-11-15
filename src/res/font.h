#ifndef LUOLA_RESOURCE_FONT_H
#define LUOLA_RESOURCE_FONT_H

#include <GL/glfw.h>

#include "resources.h"

class TextureResource;
class ProgramResource;

using std::string;

class FontResourceImpl;
class FontResource;
class DataFile;

/**
 * Helper class for setting text rendering settings
 *
 * Example usage:
 * font->text("FPS: %d", fps)       // get font renderer
 *  .color(fps < 25 ? red : green)  // select font color
 *  .pos(1, 0).align(RIGHT)         // draw at top right corner of the screen
 *  .render()                       // render text
 *
 * Note. It is not safe to keep instances of this class around for too long.
 * Subsequent calls to FontResource.text() may change the text pointer.
 */
class TextRenderer {
    friend class FontResource;
public:
    enum Alignment { LEFT, RIGHT };

    TextRenderer() = delete;

    /**
     * Set font scale.
     *
     * @param scale the scaling factor
     */
    TextRenderer &scale(float scale) { m_scale = scale; return *this; }

    /**
     * Set font color
     *
     * @param r,g,b color components
     */
    TextRenderer &color(float r, float g, float b) { m_color = glm::vec4(r,g,b,1.0f); return *this; }

    /**
     * Set font color, including transparency.
     *
     * @param color color vector
     */
    TextRenderer &color(const glm::vec4 &color) { m_color = color; return *this; }

    /**
     * Set text alignment.
     *
     * Default is LEFT, meaning the given position is the top left
     * corner of the text. If alignment is RIGHT, the position will
     * be the top right corner.
     *
     * @param align text alignment
     */
    TextRenderer &align(Alignment align) { m_align = align; return *this; }

    /**
     * Set text position.
     *
     * @param x,y text position
     */
    TextRenderer &pos(float x, float y) { m_pos = glm::vec2(x, y); return *this; }

    void render();

private:
    TextRenderer(FontResource *font, const char *text);

    FontResource *m_font;
    const char *m_text;
    float m_scale;
    glm::vec4 m_color;
    glm::vec2 m_pos;
    Alignment m_align;
};

class FontResource : public Resource {
    friend class TextRenderer;
public:

    /**
     * Load a font from a datafile
     *
     * Font shaders have the following uniforms:
     * - fontSampler (2d texture sampler)
     * - color (vec4)
     * - offset (vec2)
     * - scale (float)
     * 
     * Vertex attributes:
     *   0: vertices (vec2)
     *   1: UV coordinates
     * 
     * @param name resource name
     * @param datafile the datafile from which to load the font description
     * @param descfile the font description file name
     * @param texture the font texture
     * @param shader the shader program for rendering the font
     * @return new FontResource
     */
    static FontResource *load(const string &name, DataFile &datafile, const string &descfile, TextureResource *texture, ProgramResource *shader);

    FontResource() = delete;
    ~FontResource();

    /**
     * Render the given string.
     *
     * This function returns a TextRenderer object that allows
     * you to set additional properties, such as text position and color.
     * Call .render() to actually render the text.
     *
     * This function supports printf style formatting.
     * 
     * @param text the text to render.
     * @return TextRenderer instance
     */
    TextRenderer text(const char *text, ...);

    /**
     * Render the given string.
     *
     * This is an overload of the text(char*,...) function that
     * does not do printf style formatting.
     *
     * @param text the text to render
     * @return TextRenderer instance
     */
    TextRenderer text(const string &text);

private:
    FontResource(const string& name, FontResourceImpl *impl);

    FontResourceImpl *m_priv;
};

#endif

