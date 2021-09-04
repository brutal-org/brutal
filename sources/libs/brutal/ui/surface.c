#include <brutal/ui/surface.h>

void ui_surface_init(UiSurface *self, int width, int height)
{
    host_ui_surface_init(&self->host, width, height);
}

void ui_surface_deinit(UiSurface *self)
{
    host_ui_surface_deinit(&self->host);
}

void ui_surface_visible(UiSurface *self, bool visible)
{
    host_ui_surface_visible(&self->host, visible);
}

void ui_surface_flip(UiSurface *self, Recti rect)
{
    return host_ui_surface_flip(&self->host, rect);
}

GfxSurface ui_surface_surface(UiSurface *self)
{
    return host_ui_surface_surface(&self->host);
}
