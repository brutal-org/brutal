#include <brutal/gfx/buf.h>

void gfx_dyn_buf_init(GfxDynBuf *self, int width, int height, GfxFmt format, Alloc *alloc)
{
    *self = (GfxDynBuf){};

    self->alloc = alloc;
    self->buf.fmt = format;
    self->buf.width = width;
    self->buf.height = height;
    self->buf.pitch = gfx_fmt_size(format) * width;
    self->buf.size = gfx_fmt_size(format) * width * height;
    self->buf.buf = alloc_malloc(self->alloc, self->buf.size);
}

void gfx_dyn_buf_deinit(GfxDynBuf *self)
{
    alloc_free(self->alloc, self->buf.buf);
    *self = (GfxDynBuf){};
}

GfxBuf gfx_dyn_buf(GfxDynBuf *self)
{
    return self->buf;
}
