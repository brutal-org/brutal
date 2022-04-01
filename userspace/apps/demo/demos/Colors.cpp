#include "demo/Demo.h"

namespace Demo
{

void colors(Graphic::Painter &painter, Math::Recti screen, float time)
{
    float hue = (int)(time * 80) % 360;

    for (int x = 0; x < screen.width(); x++)
    {
        for (int y = 0; y < screen.height(); y++)
        {
            Graphic::Color color = Graphic::Color::from_hsv(hue, (y / (float)screen.height()), 1);
            painter.plot(screen.position() + Math::Vec2i(x, y), color);
        }
    }
}

} // namespace Demo