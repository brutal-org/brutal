#pragma once

#include "view.h"

typedef struct
{
    UiView base;
    GfxSurface surface;
} UiImage;

#define ui_image$(VIEW) ((UiImage *)(VIEW))

UiView *ui_image_create(GfxSurface surface);
