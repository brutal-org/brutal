#pragma once

#include <brutal/ui/view.h>

typedef struct
{
    UiView base;
} UiSplitter;

#define ui_splitter$(VIEW) ((UiSplitter *)(VIEW))

UiView *ui_splitter_create(void);
