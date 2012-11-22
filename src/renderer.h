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

