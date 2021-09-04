#pragma once

#include <brutal/base.h>
#include <brutal/gfx.h>
#include <brutal/math.h>

#if defined(__linux__)
#    include <host/linux/ui.h>
#elif defined(__brutal__)
#    include <host/brutal/ui.h>
#else
#    error "Unsupported platform!"
#endif

typedef struct host_ui_surface HostUiSurface;

void host_ui_surface_init(HostUiSurface *self, int width, int height);

void host_ui_surface_deinit(HostUiSurface *self);

void host_ui_surface_visible(HostUiSurface *self, bool visible);

void host_ui_surface_flip(HostUiSurface *self, Recti rect);

GfxSurface host_ui_surface_surface(HostUiSurface *self);
