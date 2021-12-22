#include <brutal/gfx/buf.h>

void gfx_dyn_buf_init(GfxDynBuf *self, Alloc *alloc, int width, int height, GfxFmt format)
{
    *self = (GfxDynBuf){};

    self->alloc = alloc;
    self->buf.width = width;
    self->buf.height = height;
    self->buf.fmt = format;
    self->buf.size = gfx_fmt_size(format) * width * height;
    self->buf.buf = alloc_malloc(self->alloc, self->buf.size);
}

void gfx_dyn_buf_deinit(GfxDynBuf *self)
{
    alloc_free(self->alloc, self->buf.buf);
    *self = (GfxDynBuf){};
}

GfxBuf gfx_dyn_buf_surface(GfxDynBuf *self)
{
    return self->buf;
}
