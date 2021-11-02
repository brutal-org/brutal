#include <brutal/gfx/surface.h>

void gfx_buf_init(GfxBuf *self, Alloc *alloc, int width, int height, GfxPixelFormat format)
{
    *self = (GfxBuf){};

    self->alloc = alloc;
    self->surface.width = width;
    self->surface.height = height;
    self->surface.format = format;
    self->surface.size = gfx_pixel_size(format) * width * height;
    self->surface.buf = alloc_malloc(self->alloc, self->surface.size);
}

void gfx_buf_deinit(GfxBuf *self)
{
    alloc_free(self->alloc, self->surface.buf);
    *self = (GfxBuf){};
}

GfxSurface gfx_buf_surface(GfxBuf *self)
{
    return self->surface;
}
