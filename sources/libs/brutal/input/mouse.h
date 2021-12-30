#pragma once

#include <brutal/base/std.h>

#define MSBTN_LEFT (1 << 0)
#define MSBTN_MIDDLE (1 << 1)
#define MSBTN_RIGHT (1 << 2)

typedef uint8_t MsBtns;

typedef struct
{
    int offx;
    int offy;

    int vscroll;
    int hscroll;

    MsBtns btns;
} MouseEvent;
