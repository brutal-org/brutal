#pragma once

#include <brutal-math>

typedef enum
{
    GFX_STOKE_INSIDE,
    GFX_STOKE_OUTSIDE,
    GFX_STOKE_CENTER,
} GfxStrokePos;

typedef enum
{
    GFX_STROKE_JOIN_BEVEL,
    GFX_STROKE_JOIN_MITTER,
    GFX_STROKE_JOIN_ROUND,
} GfxStrokeJoin;

typedef enum
{
    GFX_STROKE_CAP_BUTT,
    GFX_STROKE_CAP_ROUND,
    GFX_STROKE_CAP_SQUARE,
} GfxStrokeCap;

typedef struct
{
    GfxStrokePos pos;
    GfxStrokeJoin join;
    GfxStrokeCap cap;
    float width;
} GfxStroke;

void gfx_stroke_apply(GfxStroke style, MEdges *path, MEdges *stroke);
