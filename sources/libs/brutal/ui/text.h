#pragma once

#include <brutal/ui/view.h>

typedef enum
{
    UI_TEXT_BODY,
    UI_TEXT_LINK,
    UI_TEXT_HEADLINE,
} UiTextStyle;

typedef struct
{
    UiView base;

    Str text;
    UiTextStyle style;
    GfxColor color;
} UiText;

#define ui_text$(VIEW) ((UiText *)(VIEW))

UiView *ui_text_create(Str text, UiTextStyle style, GfxColor color);
