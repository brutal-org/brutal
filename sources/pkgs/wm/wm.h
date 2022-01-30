#pragma once

#include <brutal/ui.h>
#include "wm/display.h"

typedef struct
{
    WmDisplay *display;
    MVec2 mouse;
} Wm;

void wm_init(Wm *self, WmDisplay *display);

void wm_deinit(Wm *self);

void wm_dispatch(Wm *self, UiEvent event);

void wm_render(Wm *self);
