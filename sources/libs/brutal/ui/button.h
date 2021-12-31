#pragma once

#include <brutal/ui/view.h>

typedef struct
{
    UiView base;
    bool press;
    bool over;
} UiButton;

#define ui_button$(VIEW) ((UiButton *)(VIEW))

UiView *ui_button_create(void);

UiView *ui_button_create_with_text(Str text);
