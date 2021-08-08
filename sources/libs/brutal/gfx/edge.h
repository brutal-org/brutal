#pragma once

#include <brutal/ds/vec.h>
#include <brutal/math/edge.h>
#include <brutal/math/vec2.h>

typedef Vec(Edgef) GfxEdgeVec;

typedef struct
{
    GfxEdgeVec edges;
    Vec2f pos;
} GfxEdgeListBuffer;

typedef struct
{
    size_t len;
    Edgef edges[];
} GfxEdgeListInline;

typedef struct
{
    size_t len;
    Edgef *edges;
} GfxEdgeList;

void gfx_edge_list_init(GfxEdgeListBuffer *self, Alloc *alloc);

void gfx_edge_list_deinit(GfxEdgeListBuffer *self);

void gfx_edge_list_move_to(GfxEdgeListBuffer *self, Vec2f p);

void gfx_edge_list_line_to(GfxEdgeListBuffer *self, Vec2f p);

void gfx_edge_list_close(GfxEdgeListBuffer *self);
