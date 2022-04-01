#pragma once

#include <libasync/Timer.h>
#include <libwidget/Screen.h>
#include <libwidget/Window.h>

#include "neko/model/Neko.h"

namespace neko
{

struct MainWindow : public Widget::Window
{
private:
    Neko _neko;
    Box<Async::Timer> _update_timer;
    Box<Async::Observer<Neko>> _neko_observer;

public:
    MainWindow()
        : Window(WINDOW_BORDERLESS | WINDOW_TRANSPARENT | WINDOW_NO_FOCUS),
          _neko{Widget::Screen::bound().center()}
    {
        type(WINDOW_TYPE_PANEL);
        size({Neko::SIZE, Neko::SIZE});

        _update_timer = own<Async::Timer>(1000 / 8, [this]()
            { _neko.update(); });

        _neko_observer = _neko.observe([this](auto &)
            {
                position(_neko.position() - Math::Vec2i{Neko::SIZE / 2, Neko::SIZE});
                should_repaint(bound());
            });

        _update_timer->start();
    }

    void repaint(Graphic::Painter &painter, Math::Recti rectangle) override
    {
        painter.clear(rectangle, Graphic::Colors::GREEN);
        _neko.paint(painter);
    }
};

} // namespace neko
