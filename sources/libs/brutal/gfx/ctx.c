
#include <brutal/gfx/ctx.h>

void gfx_ctx_init(GfxCtx *self, Alloc *alloc, int width, int height, GfxPixelFormat format)
{
    *self = (GfxCtx){};

    gfx_buf_init(&self->buf, alloc, width, height, format);
    gfx_cache_init(&self->cache, alloc);
}

void gfx_ctx_deinit(GfxCtx *self)
{
    gfx_cache_deinit(&self->cache);
    gfx_buf_deinit(&self->buf);
}

void gfx_ctx_begin(GfxCtx *self)
{
    gfx_cache_begin(&self->cache, gfx_buf_surface(&self->buf));
}

void gfx_ctx_end(GfxCtx *self)
{
    gfx_cache_end(&self->cache, gfx_buf_surface(&self->buf));
}

GfxSurface gfx_ctx_surface(GfxCtx *self)
{
    return gfx_buf_surface(&self->buf);
}

void gfx_ctx_fill(GfxCtx *self, GfxPaint paint)
{
    self->paint = paint;
}

void gfx_ctx_no_fill(GfxCtx *self)
{
    self->paint = gfx_paint_none();
}

void gfx_ctx_stroke(GfxCtx *self, GfxStroke stroke)
{
    self->stroke = stroke;
}

void gfx_ctx_no_stroke(GfxCtx *self)
{
    self->stroke = gfx_stroke_none();
}

void gfx_ctx_clear(GfxCtx *self)
{
    Recti surface_bound = gfx_surface_bound(gfx_ctx_surface(self));
    gfx_cache_rect(
        &self->cache,
        rect$(Rectf, surface_bound),
        gfx_stroke_none(),
        self->paint,
        TRANS2_IDENTITY);
}

void gfx_ctx_rect(GfxCtx *self, Rectf rect)
{
    gfx_cache_rect(
        &self->cache,
        rect,
        self->stroke,
        self->paint,
        TRANS2_IDENTITY);
}
