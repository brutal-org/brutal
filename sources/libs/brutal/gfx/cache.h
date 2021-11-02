#pragma once

#include <brutal/ds/grid.h>
#include <brutal/ds/linear.h>
#include <brutal/gfx/paint.h>
#include <brutal/gfx/rast.h>
#include <brutal/gfx/stroke.h>
#include <brutal/hash/fnv.h>
#include <brutal/math/edge.h>

#define GFX_CACHE_CELL_SIZE (128)

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

    Rectf bound;
    Rectf clip;
    GfxStroke stroke;
    GfxPaint fill;
    Trans2 transform;
} GfxCommand;

typedef struct
{
    GfxCommand base;
    Rectf rect;
} GfxRectCommand;

typedef struct
{
    GfxCommand base;

    Vec2f start;
    Vec2f end;
} GfxLineCommad;

typedef struct
{
    GfxCommand base;

    InlineStr str;
} GfxTextCommand;

typedef struct
{
    GfxCommand base;

    InlineEdgesf edges;
} GfxPolyCommand;

typedef LinearBuf(GfxCommand) GfxCommandBuf;

typedef Grid(Fnv32) HashGrid;

typedef struct
{
    GfxCommandBuf commands;

    HashGrid front_hashgrid;
    HashGrid back_hashgrid;
} GfxCache;

void gfx_cache_init(GfxCache *self, Alloc *alloc);

void gfx_cache_deinit(GfxCache *self);

void gfx_cache_begin(GfxCache *self, GfxSurface surface);

void gfx_cache_end(GfxCache *self, GfxSurface surface);

void gfx_cache_invalidate(GfxCache *self);

void gfx_cache_rect(GfxCache *self, Rectf rect, GfxStroke stroke, GfxPaint fill, Trans2 trans);

void gfx_cache_line(GfxCache *self, Edgef line, GfxStroke stroke, Trans2 trans);

void gfx_cache_text(GfxCache *self, Str str, GfxPaint fill, Trans2 trans);

void gfx_cache_poly(GfxCache *self, Edgesf edges, GfxStroke stroke, GfxPaint fill, Trans2 trans);
