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

typedef struct embed_surface EmbedUiSurface;

void embed_surface_init(EmbedUiSurface *self, int width, int height);

void embed_surface_deinit(EmbedUiSurface *self);

void embed_surface_visible(EmbedUiSurface *self, bool visible);

void embed_surface_flip(EmbedUiSurface *self, Recti rect);

GfxSurface embed_surface_surface(EmbedUiSurface *self);
