#include <brutal/alloc/global.h>
#include <brutal/alloc/heap.h>
#include <brutal/ui/icon.h>

void ui_icon_paint(MAYBE_UNUSED UiView *self, Gfx *gfx)
{
    gfx_fill_style(gfx, gfx_paint_fill(GFX_RED));
    gfx_fill_rect(gfx, m_rect(0, 0, 100, 100), 32);

    gfx_fill_style(gfx, gfx_paint_fill(GFX_WHITE));
    gfx_text(gfx, m_vec2(16, 16), str$("BRUTAL"));
}

UiView *ui_icon_create(void)
{
    UiView *self = ui_view_create(UiIcon);

    self->repaint = ui_icon_paint;

    return self;
}
