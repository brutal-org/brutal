#pragma once

#include <brutal/ui/view.h>

typedef struct
{
    UiView base;

    Str text;
    GfxFont font;
    GfxColor color;
} UiText;

#define ui_text$(VIEW) ((UiText *)(VIEW))

UiView *ui_text_create(Str text, GfxFont font, GfxColor color);
