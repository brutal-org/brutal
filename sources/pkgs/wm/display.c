#include "wm/display.h"

void wm_display_init(WmDisplay *self, SurfaceMode mode, BalMem mem, Alloc *alloc)
{
    self->mode = mode;
    self->mem = mem;
    gfx_surface_init(&self->backbuffer, mode.width, mode.height, GFX_FMT_RGBA8888, alloc);
    gfx_init(&self->gfx, alloc);
}

void wm_display_deinit(WmDisplay *self)
{
    gfx_surface_deinit(&self->backbuffer);
    gfx_deinit(&self->gfx);
}

GfxBuf wm_display_frontbuffer(WmDisplay *self)
{
    SurfaceMode mode = self->mode;

    return (GfxBuf){
        .width = mode.width,
        .height = mode.height,
        .pitch = mode.pitch,
        .fmt = mode.format,
        .buf = self->mem.buf,
        .size = self->mem.len,
    };
}

GfxBuf wm_display_backbuffer(WmDisplay *self)
{
    return gfx_surface_buf(&self->backbuffer);
}

Gfx *wm_display_begin(WmDisplay *self)
{
    gfx_begin(&self->gfx, wm_display_backbuffer(self));

    return &self->gfx;
}

void wm_display_end(WmDisplay *self)
{
    gfx_end(&self->gfx);
    gfx_buf_copy(wm_display_frontbuffer(self), wm_display_backbuffer(self), 0, 0);
}
