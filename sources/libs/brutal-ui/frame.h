#pragma once

#include "view.h"

typedef struct
{
    UiView base;
} UiFrame;

#define ui_frame$(VIEW) ((UiFrame *)(VIEW))

UiView *ui_frame_create(void);
