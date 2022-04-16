#pragma once

#include <brutal-ds>
#include <brutal-gfx>
#include <brutal-io>

#include "spec.h"

typedef struct
{
    GfxPath path;
    MVec2f bound;
    MVec2f advance;
    bool present;
} SSFN2Glyph;

typedef struct
{
    size_t font_start;
    float weight;
    SSFN2FontHeader header;
    SSFN2StringTable stringtable;
    SSFN2Glyph *glyphs; /* glyphs array */
} SSFN2Font;

typedef Vec(SSFN2Font) SSFN2Collection;

MaybeError ssfn2_load(IoRSeek reader, SSFN2Collection *collection, Alloc *alloc);

void ssfn2_deinit(SSFN2Font *font);

GfxFont ssfn2_font(SSFN2Collection *);
