#include "splitter.h"

void ui_splitter_repaint(UiView *self, Gfx *gfx)
{
    gfx_fill_style(gfx, gfx_paint_fill(gfx_color_with_alpha(UI_COLOR_BASE09, 100)));
    gfx_fill_rect(gfx, ui_view_container(self), 0);
}

UiView *ui_splitter_create(void)
{
    UiView *self = ui_view_create$(UiSplitter);

    self->repaint = ui_splitter_repaint;
    ui_view_layout(self, "min-1");

    return self;
}
