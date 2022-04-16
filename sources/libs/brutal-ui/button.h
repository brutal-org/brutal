#pragma once

#include "view.h"

typedef enum
{
    UI_BUTTON_STYLE_DEFAULT,
    UI_BUTTON_STYLE_TEXT,
    UI_BUTTON_STYLE_FILLED,
} UiButtonStyle;

typedef struct
{
    UiView base;
    UiButtonStyle style;
    bool press;
    bool over;
} UiButton;

#define ui_button$(VIEW) ((UiButton *)(VIEW))

UiView *ui_button_create(void);

UiView *ui_button_create_with_text(Str text);
