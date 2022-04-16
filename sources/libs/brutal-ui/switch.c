#include "switch.h"

void ui_switch_repaint(UiView *self, Gfx *gfx)
{
    gfx_fill_style(gfx, gfx_paint_fill(GFX_CYAN));
    gfx_fill_rect(gfx, ui_view_container(self), 0);
}

UiView *ui_switch_create(void)
{
    UiView *self = ui_view_create$(UiSwitch);

    self->repaint = ui_switch_repaint;
    ui_view_layout(self, "max-h-36");

    return self;
}
