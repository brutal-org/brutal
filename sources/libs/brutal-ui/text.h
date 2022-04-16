#pragma once

#include "view.h"

typedef struct
{
    UiView base;

    Str text;
    GfxFontStyle font;
} UiText;

#define ui_text$(VIEW) ((UiText *)(VIEW))

UiView *ui_text_create(Str text, GfxFontStyle font);
