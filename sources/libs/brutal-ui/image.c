#include "image.h"

void ui_image_repaint(UiView *self, Gfx *gfx)
{
    GfxBuf buf = gfx_surface_buf(&ui_image$(self)->surface);
    MRectf bound = gfx_buf_bound(buf);
    gfx_fill_style(gfx, gfx_paint_image(buf, bound));

    MRectf dest = m_gravity_apply(self->layout.gravity, M_FLOW_LEFT_TO_RIGHT, bound, ui_view_content(self));
    gfx_fill_rect(gfx, dest, 0);
}

void ui_image_deinit(UiView *self)
{
    gfx_surface_deinit(&ui_image$(self)->surface);
}

UiView *ui_image_create(GfxSurface surface)
{
    UiView *self = ui_view_create$(UiImage);

    self->repaint = ui_image_repaint;
    self->deinit = ui_image_deinit;

    ui_image$(self)->surface = surface;

    return self;
}
