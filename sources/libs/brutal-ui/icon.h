#pragma once

#include "view.h"

typedef struct
{
    UiView base;
} UiIcon;

#define ui_icon$(VIEW) ((UiIcon *)(VIEW))

UiView *ui_icon_create(void);
