#pragma once

#include "view.h"

typedef struct
{
    UiView base;
} UiSpacer;

#define ui_spacer$(VIEW) ((UiSpacer *)(VIEW))

UiView *ui_spacer_create(void);
