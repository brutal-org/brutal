#include "panel.h"

void ui_panel_repaint(UiView *self, Gfx *gfx)
{
    gfx_fill_style(gfx, gfx_paint_fill(ui_panel$(self)->color));
    gfx_fill_rect(gfx, ui_view_container(self), 0);
}

UiView *ui_panel_create(GfxColor color)
{
    UiView *self = ui_view_create$(UiPanel);

    self->repaint = ui_panel_repaint;

    ui_panel$(self)->color = color;

    return self;
}
