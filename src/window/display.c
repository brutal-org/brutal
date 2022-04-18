#include "window/display.h"

void wm_display_init(WmDisplay *self, BalFb frontbuffer, Alloc *alloc)
{
    self->frontbuffer = frontbuffer;
    bal_fb_map(&self->frontbuffer);
    gfx_surface_init(&self->backbuffer, frontbuffer.width, frontbuffer.height, GFX_FMT_RGBA8888, alloc);
    gfx_init(&self->gfx, alloc);
}

void wm_display_deinit(WmDisplay *self)
{
    gfx_surface_deinit(&self->backbuffer);
    gfx_deinit(&self->gfx);
}

GfxBuf wm_display_frontbuffer(WmDisplay *self)
{
    return bal_fb_buf(&self->frontbuffer);
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

void wm_display_flip(WmDisplay *self, MRectf rect)
{
    gfx_ops_flip(wm_display_frontbuffer(self), wm_display_backbuffer(self), rect);
}

void wm_display_end(WmDisplay *self)
{
    gfx_end(&self->gfx);
}

MRectf wm_display_bound(WmDisplay *self)
{
    return gfx_buf_bound(wm_display_backbuffer(self));
}
