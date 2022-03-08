#pragma once

#include <brutal/gfx.h>
#include <brutal/math.h>
#include <ui/event.h>
#include <ui/funcs.h>
#include <ui/layout.h>

enum
{
    UI_HOOK_EFFECT,
    UI_HOOK_STATE,
    UI_HOOK_LAYOUT,
    UI_HOOK_VISUAL,
    UI_HOOK_EVENT,
    UI_HOOK_TIMER,

    UI_HOOKS_COUNT
};

static bool ui_use_effect(Ui *ui, void (*)(void *))
{
    return ui_use(ui, UI_HOOK_EFFECT) != nullptr;
}

static void *ui_use_state(Ui *ui, void *, size_t)
{
}

static MRectf ui_use_bound(Ui *ui)
{
}

static Lay *ui_use_layout(Ui *ui)
{
}

static Gfx *ui_use_visual(Ui *ui)
{
}

static Ev *ui_use_event(Ui *ui)
{
}

static bool ui_use_timer(Ui *ui, bool active, int delta)
{
}
