#pragma once

#include "view.h"

typedef struct
{
    UiView base;
    float value;
} UiSlider;

#define ui_slider$(VIEW) ((UiSlider *)(VIEW))
#define ui_slider_value$(VIEW) (ui_slider$(VIEW)->value)

UiView *ui_slider_create(void);
