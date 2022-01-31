#include <brutal/gfx/buf.h>

void gfx_buf_init(GfxBuf *self, int width, int height, GfxFmt format, int pitch, void *buf)
{
    self->fmt = format;
    self->fmt_bpp = gfx_fmt_size(self->fmt);
    switch (self->fmt)
    {
    case GFX_FMT_GS8:
        self->fmt_load_impl = gfx_fmt_load_gs8;
        self->fmt_store_impl = gfx_fmt_store_gs8;
        break;
    case GFX_FMT_RGB888:
        self->fmt_load_impl = gfx_fmt_load_rgb888;
        self->fmt_store_impl = gfx_fmt_store_rgb888;
        break;
    case GFX_FMT_RGBA8888:
        self->fmt_load_impl = gfx_fmt_load_rgba8888;
        self->fmt_store_impl = gfx_fmt_store_rgba8888;
        break;
    case GFX_FMT_BGRA8888:
        self->fmt_load_impl = gfx_fmt_load_bgra8888;
        self->fmt_store_impl = gfx_fmt_store_bgra8888;
        break;
    default:
        panic$("Unknown pixel format {}", self->fmt);
        break;
    }

    self->width = width;
    self->height = height;
    self->pitch = pitch;
    self->size = self->pitch * height;
    self->buf = buf;
}

void gfx_dyn_buf_init(GfxDynBuf *self, int width, int height, GfxFmt format, Alloc *alloc)
{
    *self = (GfxDynBuf){};

    self->alloc = alloc;
    int pitch = gfx_fmt_size(format) * width;
    size_t size = pitch * height;
    void *buf = alloc_malloc(self->alloc, size);
    gfx_buf_init(&self->buf, width, height, format, pitch, buf);
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
