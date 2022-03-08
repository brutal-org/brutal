#pragma once

#include <ui/macros.h>

static bool ui_button_begin(Ui *ui)
{
    ui_begin$(ui);

    struct State
    {
        bool over;
    };

    struct State *state = ui_use_state$(ui, (struct State){});

    Gfx *gfx;
    if ((gfx = ui_use_visual(ui)))
    {
        gfx_paint(gfx, state->over ? GFX_LIGHTBLUE : GFX_BLUE);
        gfx_fill_rect(gfx, ui_use_bound(ui));
    }

    Ev *ev;
    if ((ev = ui_use_event(ui)))
    {
        if (ev->type == EV_ENTER)
        {
            state->over = true;
            ui_update(ui, UI_VISUAL);
        }
        else if (ev->type == EV_LEAVE)
        {
            state->over = false;
            ui_update(ui, UI_VISUAL);
        }
    }
}
