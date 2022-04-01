#pragma once

#include <libwidget/Application.h>

#include "demo/Window.h"

namespace Demo
{

struct Application : public Widget::Application
{
    Box<Widget::Window> build() override
    {
        return own<Window>();
    }
};

} // namespace Demo