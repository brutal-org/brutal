#include <brutal/gfx/surface.h>

void gfx_buffer_init(GfxBuffer *self, Alloc *alloc, int width, int height, GfxPixelFormat format)
{
    *self = (GfxBuffer){};

    self->alloc = alloc;
    self->surface.width = width;
    self->surface.height = height;
    self->surface.format = format;
    self->surface.size = gfx_pixel_size(format) * width * height;
    self->surface.buffer = alloc_malloc(self->alloc, self->surface.size);
}

void gfx_buffer_deinit(GfxBuffer *self)
{
    alloc_free(self->alloc, self->surface.buffer);
    *self = (GfxBuffer){};
}

GfxSurface gfx_buffer_surface(GfxBuffer *self)
{
    return self->surface;
}
