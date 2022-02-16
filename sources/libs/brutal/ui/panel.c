#include <brutal/alloc/global.h>
#include <brutal/ui/panel.h>

void ui_panel_paint(UiView *self, Gfx *gfx)
{
    gfx_fill_style(gfx, gfx_paint_fill(ui_panel$(self)->color));
    gfx_fill_rect(gfx, ui_view_container(self), 0);
}

UiView *ui_panel_create(GfxColor color)
{
    UiView *self = ui_view_create$(UiPanel);

    self->repaint = ui_panel_paint;

    ui_panel$(self)->color = color;

    return self;
}
