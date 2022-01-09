#pragma once

#include <brutal/ui/view.h>

typedef struct
{
    UiView base;
    GfxColor color;
    Str text;
} UiText;

#define ui_text$(VIEW) ((UiText *)(VIEW))

UiView *ui_text_create(Str text, GfxColor color);
