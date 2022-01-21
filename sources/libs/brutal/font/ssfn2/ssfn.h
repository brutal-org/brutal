#pragma once
#include <brutal/base.h>
#include <brutal/io/traits.h>

typedef struct PACKED
{
    le_uint8_t magic[4];         /* SSFN magic bytes */
    le_uint32_t size;            /* total size in bytes */
} SSFN2CommonHeader;

typedef struct PACKED
{
    le_uint8_t type;             /* font family and style */
    le_uint8_t features;         /* format features and revision */
    le_uint8_t width;            /* overall width of the font */
    le_uint8_t height;           /* overall height of the font */
    le_uint8_t baseline;         /* horizontal baseline in grid pixels */
    le_uint8_t underline;        /* position of under line in grid pixels */
    le_uint16_t fragments_offs;  /* offset of fragments table */
    le_uint32_t characters_offs; /* characters table offset */
    le_uint32_t ligature_offs;   /* ligatures table offset */
    le_uint32_t kerning_offs;    /* kerning table offset */
    le_uint32_t cmap_offs;       /* color map offset */
} SSFN2FontHeader;

MaybeError ssfn2_load(IoRSeek reader);
