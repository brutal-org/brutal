#pragma once

#include <brutal-text>

#define SSFN2_MAGIC str$("SFN2")
#define SSFN2_COLLECTION str$("SFNC")
#define SSFN2_ENDMAGIC str$("2NFS")

/* font family group in font type byte */
#define SSFN2_TYPE_FAMILY(x) ((x)&15)
#define SSFN2_FAMILY_SERIF 0
#define SSFN2_FAMILY_SANS 1
#define SSFN2_FAMILY_DECOR 2
#define SSFN2_FAMILY_MONOSPACE 3
#define SSFN2_FAMILY_HAND 4

/* font style flags in font type byte */
#define SSFN2_TYPE_STYLE(x) (((x) >> 4) & 15)
#define SSFN2_STYLE_REGULAR 0
#define SSFN2_STYLE_BOLD 1
#define SSFN2_STYLE_ITALIC 2
#define SSFN2_STYLE_USRDEF1 4 /* user defined variant 1 */
#define SSFN2_STYLE_USRDEF2 8 /* user defined variant 2 */

enum
{
    SSFN2_CMD_MOVE_TO,
    SSFN2_CMD_LINE_TO,
    SSFN2_CMD_QUAD_CURVE,
    SSFN2_CMD_BEZIER_CURVE,
};

typedef struct PACKED
{
    le_uint8_t magic[4];
    le_uint32_t size; /* total size in bytes */
} SSFN2CommonHeader;

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
