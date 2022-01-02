#pragma once

#include <brutal/math/edge.h>
#include <brutal/math/vec2.h>
#include <brutal/parse/scan.h>

typedef enum
{
    GFX_CMD_NOP,

    GFX_CMD_MOVE_TO,
    GFX_CMD_CLOSE_PATH,
    GFX_CMD_LINE_TO,
    GFX_CMD_CUBIC_TO,
    GFX_CMD_QUADRATIC_TO,
    GFX_CMD_ARC_TO,
} GfxPathType;

typedef struct
{
    GfxPathType type;
    bool rel;

#define GFX_PATH_LARGE (1 << 0)
#define GFX_PATH_SWEEP (1 << 1)
    int flags;
    float rx;
    float ry;
    float angle;
    MVec2 cp1;
    union
    {
        MVec2 cp2;
        MVec2 cp;
    };
    MVec2 point;
} GfxPathCmd;

#define GFX_PATH_CMDS "MmZzLlHhVvCcSsQqTtAa"
#define GFX_PATH_WS "\n\r\t "

typedef struct
{
    MVec2 start;
    MVec2 cp;
    MVec2 last;
    void (*eval)(GfxPathCmd cmd, void *ctx);
    void *ctx;
} GfxPathParser;

void gfx_path_parse(GfxPathParser *self, Scan *scan);

typedef struct
{
    MVec2 start;
    MVec2 last;
    void (*append)(MEdge edge, void *ctx);
    void *ctx;
} GfxPathFlattener;

#define GFX_FLATTEN_MAX_DEPTH (16)
#define GFX_FLATTEN_TOLERANCE (0.25f)

void gfx_path_flatten(GfxPathFlattener *self, GfxPathCmd cmd);
