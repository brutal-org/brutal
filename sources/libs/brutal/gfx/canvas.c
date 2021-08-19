
#include <brutal/gfx/canvas.h>

void gfx_canvas_init(GfxCanvas *self, Alloc *alloc, int width, int height, GfxPixelFormat format)
{
    *self = (GfxCanvas){};

    gfx_buffer_init(&self->buffer, alloc, width, height, format);
    gfx_rast_init(&self->rast, alloc);
    gfx_cache_init(&self->cache, alloc);
}

void gfx_canvas_deinit(GfxCanvas *self)
{
    gfx_cache_deinit(&self->cache);
    gfx_rast_deinit(&self->rast);
    gfx_buffer_deinit(&self->buffer);
}

void gfx_canvas_begin(GfxCanvas *self)
{
    gfx_cache_begin(&self->cache, gfx_buffer_surface(&self->buffer));
}

void gfx_canvas_end(GfxCanvas *self)
{
    gfx_cache_end(&self->cache, &self->rast, gfx_buffer_surface(&self->buffer));
}

GfxSurface gfx_canvas_surface(GfxCanvas *self)
{
    return gfx_buffer_surface(&self->buffer);
}

void gfx_canvas_fill(GfxCanvas *self, GfxPaint paint)
{
    self->paint = paint;
}

void gfx_canvas_no_fill(GfxCanvas *self)
{
    self->paint = gfx_paint_none();
}

void gfx_canvas_stroke(GfxCanvas *self, GfxStroke stroke)
{
    self->stroke = stroke;
}

void gfx_canvas_no_stroke(GfxCanvas *self)
{
    self->stroke = gfx_stroke_none();
}

void gfx_canvas_clear(GfxCanvas *self)
{
    auto surface_bound = gfx_surface_bound(gfx_canvas_surface(self));
    gfx_cache_rect(
        &self->cache,
        rect$(Rectf, surface_bound),
        gfx_stroke_none(),
        self->paint,
        gfx_transform_identity());
}

void gfx_canvas_rect(GfxCanvas *self, Rectf rect)
{
    gfx_cache_rect(
        &self->cache,
        rect,
        self->stroke,
        self->paint,
        gfx_transform_identity());
}
