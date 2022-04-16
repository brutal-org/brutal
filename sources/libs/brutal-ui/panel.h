#pragma once

#include "view.h"

typedef struct
{
    UiView base;
    GfxColor color;
} UiPanel;

#define ui_panel$(VIEW) ((UiPanel *)(VIEW))

UiView *ui_panel_create(GfxColor color);
