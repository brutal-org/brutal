#include <libgraphic/Framebuffer.h>
#include <libutils/Vec.h>

#include "compositor/Cursor.h"
#include "compositor/Manager.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

#include "compositor/model/Wallpaper.h"

static Box<Graphic::Framebuffer> _framebuffer;
static Box<compositor::Wallpaper> _wallpaper;

static Vec<Math::Recti> _dirty_regions;

static Box<Settings::Setting> _night_light_enable_setting;
bool _night_light_enable = false;

static Box<Settings::Setting> _night_light_stenght_setting;
double _night_light_strenght = 0.5;

void renderer_initialize()
{
    _framebuffer = Graphic::Framebuffer::open().unwrap();
    _wallpaper = own<compositor::Wallpaper>(_framebuffer->resolution().size());
    _wallpaper->on_change = []()
    { renderer_region_dirty(renderer_bound()); };

    _night_light_enable_setting = own<Settings::Setting>("appearance:night-light.enable", [](auto &value)
        {
            _night_light_enable = value.as_bool();
            renderer_region_dirty(renderer_bound());
        });

    _night_light_stenght_setting = own<Settings::Setting>("appearance:night-light.stenght", [](auto &value)
        {
            _night_light_strenght = value.as_double();
            renderer_region_dirty(renderer_bound());
        });

    renderer_region_dirty(_framebuffer->resolution());
}

void renderer_region_dirty(Math::Recti new_region)
{
    if (new_region.is_empty())
    {
        return;
    }

    bool merged = false;

    _dirty_regions.foreach([&](Math::Recti &region)
        {
            if (region.colide_with(new_region))
            {
                Math::Recti top;
                Math::Recti botton;
                Math::Recti left;
                Math::Recti right;

                new_region.substract(region, top, botton, left, right);

                renderer_region_dirty(top);
                renderer_region_dirty(botton);
                renderer_region_dirty(left);
                renderer_region_dirty(right);

                merged = true;

                return Iter::STOP;
            }
            else
            {
                return Iter::CONTINUE;
            }
        });

    if (!merged)
    {
        _dirty_regions.push_back(new_region);
    }
}

void renderer_composite_wallpaper(Graphic::Painter &painter, Math::Recti region)
{
    painter.blit(_wallpaper->scaled(), region, region);
    _framebuffer->mark_dirty(region);
}

void renderer_composite_region(Graphic::Painter &painter, Math::Recti region, Window *window_transparent)
{
    renderer_composite_wallpaper(painter, region);

    manager_iterate_back_to_front([&](Window *window)
        {
            if (window == window_transparent)
            {
                return Iter::STOP;
            }

            if (window->bound().colide_with(region))
            {
                Math::Recti destination = window->bound().clipped_with(region);

                Math::Recti source(
                    destination.position() - window->bound().position(),
                    destination.size());

                if (window->flags() & WINDOW_ACRYLIC)
                {
                    painter.blit(_wallpaper->acrylic(), destination, destination);
                }

                painter.blit(window->frontbuffer(), source, destination);
            }

            return Iter::CONTINUE;
        });

    _framebuffer->mark_dirty(region);
}

void renderer_region(Graphic::Painter &painter, Math::Recti region)
{
    bool should_paint_wallpaper = true;

    if (region.is_empty())
    {
        return;
    }

    manager_iterate_front_to_back([&](Window *window)
        {
            if (window->bound().colide_with(region))
            {
                Math::Recti destination = window->bound().clipped_with(region);

                Math::Recti source(
                    destination.position() - window->bound().position(),
                    destination.size());

                int radius = 6;

                if (window->flags() & WINDOW_TRANSPARENT)
                {
                    renderer_composite_region(painter, destination, window);
                    painter.blit(window->frontbuffer(), source, destination);
                }
                else if (window->flags() & WINDOW_ACRYLIC)
                {
                    if (window->flags() & WINDOW_NO_ROUNDED_CORNERS)
                    {
                        painter.blit(_wallpaper->acrylic(), region, region);
                        painter.blit(window->frontbuffer(), source, destination);
                    }
                    else
                    {
                        painter.push();
                        painter.clip(destination);

                        renderer_composite_region(painter, window->bound().take_top_left(radius).clipped_with(region), window);
                        renderer_composite_region(painter, window->bound().take_top_right(radius).clipped_with(region), window);
                        renderer_composite_region(painter, window->bound().take_bottom_left(radius).clipped_with(region), window);
                        renderer_composite_region(painter, window->bound().take_bottom_right(radius).clipped_with(region), window);

                        painter.blit_rounded(_wallpaper->acrylic(), window->bound(), window->bound(), radius);
                        painter.blit_rounded(window->frontbuffer(), window->bound().size(), window->bound(), radius);
                        painter.pop();
                    }
                }
                else
                {
                    if (window->flags() & WINDOW_NO_ROUNDED_CORNERS)
                    {
                        painter.blit(window->frontbuffer(), source, destination);
                    }
                    else
                    {
                        painter.push();
                        painter.clip(destination);

                        renderer_composite_region(painter, window->bound().take_top_left(radius).clipped_with(region), window);
                        renderer_composite_region(painter, window->bound().take_top_right(radius).clipped_with(region), window);
                        renderer_composite_region(painter, window->bound().take_bottom_left(radius).clipped_with(region), window);
                        renderer_composite_region(painter, window->bound().take_bottom_right(radius).clipped_with(region), window);

                        painter.blit_rounded(window->frontbuffer(), window->bound().size(), window->bound(), radius);
                        painter.pop();
                    }
                }

                _framebuffer->mark_dirty(destination);

                Math::Recti top;
                Math::Recti botton;
                Math::Recti left;
                Math::Recti right;

                region.substract(destination, top, botton, left, right);

                renderer_region(painter, top);
                renderer_region(painter, botton);
                renderer_region(painter, left);
                renderer_region(painter, right);

                should_paint_wallpaper = false;

                return Iter::STOP;
            }

            return Iter::CONTINUE;
        });

    if (should_paint_wallpaper)
    {
        renderer_composite_wallpaper(painter, region);
    }
}

Math::Recti renderer_bound()
{
    return _framebuffer->resolution();
}

void renderer_repaint_dirty()
{
    Graphic::Painter painter{_framebuffer->bitmap()};

    _dirty_regions.foreach([&](Math::Recti region)
        {
            renderer_region(painter, region);

            if (_night_light_enable)
            {
                painter.tint(region, Graphic::Color::from_rgb(1, 0.9, 0.8));
            }

            if (region.colide_with(cursor_bound()))
            {
                renderer_region(painter, cursor_bound());

                cursor_render(painter);

                if (_night_light_enable)
                {
                    painter.tint(cursor_bound(), Graphic::Color::from_rgb(1, 0.9, 0.8));
                }
            }

            return Iter::CONTINUE;
        });

    _framebuffer->blit();

    _dirty_regions.clear();
}

bool renderer_set_resolution(int width, int height)
{
    auto result = _framebuffer->set_resolution(Math::Vec2i(width, height));

    if (result != SUCCESS)
    {
        return false;
    }

    _wallpaper->change_resolution({width, height});
    renderer_region_dirty(renderer_bound());

    return true;
}

void renderer_set_wallaper(Ref<Graphic::Bitmap>)
{
    renderer_region_dirty(renderer_bound());
}
