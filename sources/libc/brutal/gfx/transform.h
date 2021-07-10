#pragma once

#include <brutal/math/mat3x2.h>
#include <brutal/math/vec2.h>

typedef struct
{
    Vec2f translation;
    Vec2f origine;
    Mat3x2f matrix;
} GfxTransform;

#define gfx_transform_identity() \
    (GfxTransform) { .matrix = mat3x2_indentity(Mat3x2f) }
