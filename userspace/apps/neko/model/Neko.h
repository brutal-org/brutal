#pragma once

#include <libasync/Observable.h>
#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>

#include "neko/graphics/Sprites.h"
#include "neko/model/Behavior.h"

namespace neko
{

struct Neko : public Async::Observable<Neko>
{
private:
    int _tick = 0;
    Math::Vec2f _position;
    Ref<Graphic::Bitmap> _sprites;

    Box<Behavior> _behavior;
    Box<Behavior> _next_behavior;

    Math::Recti sprite();

public:
    static constexpr int SIZE = 32;
    static constexpr int SPEED = 16;

    int tick() { return _tick; }

    Math::Vec2f position() { return _position; }

    Neko(Math::Vec2f starting_position);

    void update();

    void paint(Graphic::Painter &painter);

    void move_to(Math::Vec2f position);

    void behavior(Box<Behavior>);
};

} // namespace neko
