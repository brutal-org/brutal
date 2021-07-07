#pragma once

#include <brutal/math/Mat3x2.h>
#include <brutal/math/Vec2.h>

typedef struct
{
    Vec2f translation;
    Vec2f origine;
    Mat3x2f matrix;
} GfxTransform;
