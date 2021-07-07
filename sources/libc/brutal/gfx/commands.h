#pragma once

#include <brutal/ds/linear.h>
#include <brutal/gfx/edge.h>
#include <brutal/gfx/paint.h>
#include <brutal/gfx/rast.h>
#include <brutal/gfx/stroke.h>

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
    GfxTransform transform;
} GfxCommand;

typedef struct
{
    GfxCommand base;
    Recti rect;
} GfxRectCommand;

typedef struct
{
    GfxCommand base;

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

    GfxEdgeListInline edges;
} GfxPolyCommand;

typedef LinearBuffer(GfxCommand) GfxCommandBuffer;

void gfx_command_init(GfxCommandBuffer *self);

void gfx_command_deinit(GfxCommandBuffer *self);

void gfx_command_rect(GfxCommandBuffer *self, Rectf rect, GfxStroke stroke, GfxPaint fill, GfxTransform transform);

void gfx_command_line(GfxCommandBuffer *self, Edgef line, GfxStroke stroke, GfxTransform transform);

void gfx_command_text(GfxCommandBuffer *self, Str str, GfxPaint fill, GfxTransform transform);

void gfx_command_poly(GfxCommandBuffer *self, GfxEdgeList edges, GfxStroke stroke, GfxPaint fill, GfxTransform transform);

void gfx_command_eval(GfxCommandBuffer *self, GfxRast *rast, GfxSurface *surface);
