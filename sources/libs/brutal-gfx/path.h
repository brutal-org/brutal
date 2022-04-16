#pragma once

#include <brutal-math>
#include <brutal-parse>

/* --- Path Object ---------------------------------------------------------- */

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
    MVec2f cp1;
    union
    {
        MVec2f cp2;
        MVec2f cp;
    };
    MVec2f point;
} GfxPathCmd;

typedef Vec(GfxPathCmd) GfxPath;

void gfx_path_init(GfxPath *path, Alloc *alloc);

void gfx_path_deinit(GfxPath *path);

void gfx_path_dump(GfxPath *path);

void gfx_path_move_to(GfxPath *path, MVec2f p);

void gfx_path_close(GfxPath *path);

void gfx_path_line_to(GfxPath *path, MVec2f p);

void gfx_path_cubic_to(GfxPath *path, MVec2f cp1, MVec2f cp2, MVec2f p);

void gfx_path_quadratic_to(GfxPath *path, MVec2f cp, MVec2f p);

void gfx_path_arc_to(GfxPath *path, float rx, float ry, float angle, int flags, MVec2f p);

/* --- Parser --------------------------------------------------------------- */

#define GFX_PATH_CMDS "MmZzLlHhVvCcSsQqTtAa"
#define GFX_PATH_WS "\n\r\t "

typedef struct
{
    MVec2f start;
    MVec2f cp;
    MVec2f last;
    void (*eval)(void *ctx, GfxPathCmd cmd);
    void *ctx;
} GfxPathParser;

void gfx_path_parse(GfxPathParser *self, Scan *scan);

/* --- Flattener ------------------------------------------------------------ */

typedef struct
{
    MVec2f start;
    MVec2f last;
    void (*append)(void *ctx, MEdge edge);
    void *ctx;
} GfxPathFlattener;

#define GFX_FLATTEN_MAX_DEPTH (16)

#define GFX_FLATTEN_TOLERANCE (0.25f)

void gfx_flattener_begin(GfxPathFlattener *self);

void gfx_flattener_flatten(GfxPathFlattener *self, GfxPathCmd cmd);
