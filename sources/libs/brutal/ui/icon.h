#pragma once

#include <brutal/codec/ssfn2/ssfn2.h>
#include <brutal/ui/view.h>

typedef struct
{
    UiView base;
} UiIcon;

#define ui_icon$(VIEW) ((UiIcon *)(VIEW))

UiView *ui_icon_create(void);
