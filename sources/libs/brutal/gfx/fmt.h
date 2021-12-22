#pragma once

#include <brutal/base/attributes.h>
#include <brutal/debug/assert.h>
#include <brutal/gfx/color.h>

typedef enum
{
    GFX_FMT_UNKNOWN,

    GFX_FMT_GS8,
    GFX_FMT_RGB888,
    GFX_FMT_RGBA8888,
    GFX_FMT_BGRA8888,
} GfxFmt;

void gfx_fmt_store(GfxFmt self, GfxColor color, void *dst);

GfxColor gfx_fmt_load(GfxFmt self, const void *src);

size_t gfx_fmt_size(GfxFmt self);
