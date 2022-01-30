#include "wm/wm.h"

void wm_init(Wm *self, WmDisplay *display)
{
    self->display = display;
    self->mouse = m_vec2(64, 64);
}

void wm_deinit(Wm *)
{
}

void wm_dispatch(Wm *self, UiEvent event)
{
    if (event.type == UI_EVENT_MOUSE_MOVE)
    {
        self->mouse = m_vec2_add(self->mouse, event.mouse.offset);
        wm_render(self);
    }
}

void wm_render(Wm *self)
{
    Gfx *gfx = wm_display_begin(self->display);

    gfx_clear(gfx, GFX_GAINSBORO);

    gfx_color(gfx, GFX_BLACK);
    gfx_fill(gfx, gfx_paint_fill(GFX_BLACK));

    // Draw the mouse cursor.
    gfx_push(gfx);
    gfx_origin(gfx, self->mouse);
    gfx_fill_svg(gfx, str$("M 0 0 L 16 12.279 L 9.049 13.449 L 13.374 22.266 L 9.778 24 L 5.428 15.121 L -0 19.823 Z"));
    gfx_pop(gfx);

    wm_display_end(self->display);
}
