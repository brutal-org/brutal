#pragma once

#include <brutal-ds>
#include <brutal-math>

typedef Vec(MRectf) GfxDirty;

void gfx_dirty_init(GfxDirty *self, Alloc *alloc);

void gfx_dirty_deinit(GfxDirty *self);

void gfx_dirty_rect(GfxDirty *self, MRectf rect);

bool gfx_dirty_any(GfxDirty *self);

void gfx_dirty_clear(GfxDirty *self);

#define gfx_dirty_foreach vec_foreach_v
