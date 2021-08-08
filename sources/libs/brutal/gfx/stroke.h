#pragma once

typedef enum
{
    GFX_STROKE_NONE,
    GFX_STROKE_LINE,
} GfxStrokeType;

typedef enum
{
    GFX_LINE_CAP_BUTT,
    GFX_LINE_CAP_ROUND,
    GFX_LINE_CAP_SQUARE,
} GfxLineCap;

typedef enum
{
    GFX_LINE_JOIN_MITER,
    GFX_LINE_JOIN_BEVEL,
    GFX_LINE_JOIN_ROUND,
} GfxLineJoin;

typedef struct
{
    float width;
    GfxLineCap line_cap;
    GfxLineJoin line_join;
} GfxStroke;

#define gfx_stroke_none() \
    (GfxStroke) {}
