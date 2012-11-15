#ifndef LUOLA_RENDERER_H
#define LUOLA_RENDERER_H

class World;
class FontResource;

class Renderer {
public:
    Renderer(const World &world);

    void render(double frametime) const;

private:
    const World &m_world;
    FontResource *m_font;
};

#endif

