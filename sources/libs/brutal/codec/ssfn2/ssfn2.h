#pragma once

#include <brutal/ds.h>
#include <brutal/gfx.h>
#include <brutal/io.h>

typedef struct PACKED
{
    le_uint8_t type;     /* font family and style */
    le_uint8_t features; /* format features and revision */

    le_uint8_t width;     /* overall width of the font */
    le_uint8_t height;    /* overall height of the font */
    le_uint8_t baseline;  /* horizontal baseline in grid pixels */
    le_uint8_t underline; /* position of under line in grid pixels */

    le_uint16_t fragments_offs;  /* offset of fragments table */
    le_uint32_t characters_offs; /* characters table offset */
    le_uint32_t ligature_offs;   /* ligatures table offset */
    le_uint32_t kerning_offs;    /* kerning table offset */
    le_uint32_t cmap_offs;       /* color map offset */
} SSFN2FontHeader;

#define SSFN2_MAX_STR_LEN 255

typedef struct PACKED
{
    char font_name[SSFN2_MAX_STR_LEN];
    char family_name[SSFN2_MAX_STR_LEN];
    char subfamily_name[SSFN2_MAX_STR_LEN];
    char revision[SSFN2_MAX_STR_LEN];
    char manufacturer[SSFN2_MAX_STR_LEN];
    char license[SSFN2_MAX_STR_LEN];
} SSFN2StringTable;

typedef struct
{
    GfxPath path;
    uint8_t width;
    uint8_t height;
    uint8_t adv_x;
    uint8_t adv_y;
    bool present;
} SSFN2Glyph;

typedef struct SSFN2Font
{
    size_t font_start;
    SSFN2FontHeader header;
    SSFN2StringTable stringtable;
    SSFN2Glyph *glyphs; /* glyphs array */
} SSFN2Font;

typedef Vec(SSFN2Font) SSFN2Collection;

MaybeError ssfn2_load(IoRSeek reader, SSFN2Collection *collection, Alloc *alloc);

void font_ssfn2_deinit(SSFN2Font *font);

/* --- SSFN2 Font ----------------------------------------------------------- */

GfxFont gfx_font_ssfn2(SSFN2Collection *);
