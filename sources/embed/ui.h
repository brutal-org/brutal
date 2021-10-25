#pragma once

#include <brutal/base.h>
#include <brutal/gfx.h>
#include <brutal/math.h>

#if defined(SYSTEM_SDL)
#    include <embed/sdl/ui.h>
#elif defined(SYSTEM_BRUTAL)
#    include <embed/brutal/ui.h>
#else
#    error "Unsupported embedder!"
#endif

typedef struct host_ui_surface HostUiSurface;

void host_ui_surface_init(HostUiSurface *self, int width, int height);

void host_ui_surface_deinit(HostUiSurface *self);

void host_ui_surface_visible(HostUiSurface *self, bool visible);

void host_ui_surface_flip(HostUiSurface *self, Recti rect);

GfxSurface host_ui_surface_surface(HostUiSurface *self);
