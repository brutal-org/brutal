#pragma once

#include <brutal/app.h>
#include <embed/ui.h>

typedef struct
{
    HostUiSurface host;
} UiSurface;

void ui_surface_init(UiSurface *self, int width, int height);

void ui_surface_deinit(UiSurface *self);

void ui_surface_visible(UiSurface *self, bool visible);

void ui_surface_flip(UiSurface *self, Recti rect);

GfxSurface ui_surface_surface(UiSurface *self);
