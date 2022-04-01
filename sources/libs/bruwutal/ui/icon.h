#pragma once

#include <bruwutal/codec/ssfn2/ssfn2.h>
#include <bruwutal/ui/view.h>

typedef struct
{
    UiView base;
} UiIcon;

#define ui_icon$(VIEW) ((UiIcon *)(VIEW))

UiView *ui_icon_create(void);
