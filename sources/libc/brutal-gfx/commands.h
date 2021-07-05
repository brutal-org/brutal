#pragma once

#include <brutal/math/Rect.h>
#include <brutal/math/Vec2.h>
#include <brutal/text/str.h>
#include <brutal-gfx/paint.h>
#include <brutal-gfx/stroke.h>

typedef enum
{
    GFX_COMMAND_NOOP,

    GFX_COMMAND_RECT,
    GFX_COMMAND_LINE,
    GFX_COMMAND_TEXT,
    GFX_COMMAND_POLY,
} GfxCommandType;

typedef struct
{
    GfxCommandType type;
    Recti bound;

    GfxStroke stroke;
    GfxPaint fill;
} GfxCommand;

typedef struct
{
    GfxCommand base;
    Recti rect;
} GfxRectCommand;

typedef struct
{
    GfxCommand base;

    int thickness;
    Vec2i start;
    Vec2i end;
} GfxLineCommad;

typedef struct
{
    GfxCommand base;

    InlineStr str;
} GfxTextCommand;

typedef struct
{
    GfxCommand base;

    size_t count;
    Vec2f points[];
} GfxPolyCommand;

void gfx_eval_command(GfxCommand *command, GfxSurface *surface);
