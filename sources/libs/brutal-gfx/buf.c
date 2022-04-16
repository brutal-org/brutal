#include "buf.h"

void gfx_surface_init(GfxSurface *self, int width, int height, GfxFmt format, Alloc *alloc)
{
    *self = (GfxSurface){};

    self->alloc = alloc;
    self->buf.fmt = format;
    self->buf.width = width;
    self->buf.height = height;
    self->buf.pitch = gfx_fmt_size(format) * width;
    self->buf.buf = alloc_malloc(self->alloc, gfx_fmt_size(format) * width * height);
}

void gfx_surface_deinit(GfxSurface *self)
{
    alloc_free(self->alloc, self->buf.buf);
    *self = (GfxSurface){};
}

GfxBuf gfx_surface_buf(GfxSurface *self)
{
    return self->buf;
}
