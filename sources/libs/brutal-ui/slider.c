#include "slider.h"

void ui_slider_repaint(UiView *self, Gfx *gfx)
{
    MRectf track = ui_view_container(self);
    MRectf numb = m_rectf(0, 0, track.size.x, track.size.y / 2);

    gfx_fill_style(gfx, gfx_paint_fill(UI_COLOR_BASE09));
    gfx_fill_rect(gfx, track, 8);
    gfx_fill_style(gfx, gfx_paint_fill(UI_COLOR_BLUE));
    gfx_fill_rect(gfx, numb, 8);
}

UiView *ui_slider_create(void)
{
    UiView *self = ui_view_create$(UiSlider);

    self->repaint = ui_slider_repaint;

    return self;
}
