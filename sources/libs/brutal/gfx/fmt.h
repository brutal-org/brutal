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

void gfx_fmt_store_gs8(GfxColor color, void *dst);
void gfx_fmt_store_rgb888(GfxColor color, void *dst);
void gfx_fmt_store_rgba8888(GfxColor color, void *dst);
void gfx_fmt_store_bgra8888(GfxColor color, void *dst);

GfxColor gfx_fmt_load_gs8(const void *src);
GfxColor gfx_fmt_load_rgb888(const void *src);
GfxColor gfx_fmt_load_rgba8888(const void *src);
GfxColor gfx_fmt_load_bgra8888(const void *src);

size_t gfx_fmt_size(GfxFmt self);
