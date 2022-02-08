#include <brutal/alloc/global.h>
#include <brutal/ui/icon.h>

void ui_icon_paint(MAYBE_UNUSED UiView *self, Gfx *gfx)
{
    gfx_fill(gfx, gfx_paint_fill(GFX_BLACK));
    gfx_fill_rect(gfx, ui_view_container(self), 0);

    gfx_fill(gfx, gfx_paint_fill(GFX_RED));
    gfx_fill_rect(gfx, m_rect(100, 100, 100, 100), 32);

    gfx_fill(gfx, gfx_paint_fill(GFX_GREEN));
    gfx_stroke(gfx, (GfxStroke){
                        .pos = GFX_STOKE_CENTER,
                        .join = GFX_STROKE_JOIN_BEVEL,
                        .width = 5,
                    });
    gfx_stroke_path(gfx);
}

UiView *ui_icon_create(void)
{
    UiView *self = ui_view_create(UiIcon);

    self->paint = ui_icon_paint;

    return self;
}
