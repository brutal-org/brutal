#pragma once

#include <brutal/math/vec2.h>

typedef struct
{
    float xx;
    float xy;
    float yx;
    float yy;

    float ox;
    float oy;
} Trans2;

#define TRANS2_IDENTITY \
    (Trans2) { 1, 0, 0, 1, 0, 0 }
