#pragma once

#include "view.h"

typedef struct
{
    UiView base;
    bool value;
} UiSwitch;

#define ui_switch$(VIEW) ((UiSwitch *)(VIEW))
#define ui_switch_value$(VIEW) (ui_switch$(VIEW)->value)

UiView *ui_switch_create(void);
