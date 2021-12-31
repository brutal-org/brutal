#pragma once

#include <brutal/font/metrics.h>
#include <brutal/gfx.h>
#include <brutal/text.h>

typedef enum
{
    BFONT_BUILTIN,
} BFontType;

typedef struct
{
    BFontType type;
} BFont;

float bfont_advance(BFont *font, Rune rune);

BFontMetrics bfont_metrics(BFont *font);

BFontMesure bfont_mesure(BFont *font, Str str);

void bfont_render_rune(BFont *font, Rune rune, MVec2 baseline, GfxBuf buf, MRect clip, GfxColor color);

void bfont_render_str(BFont *font, Str str, MVec2 baseline, GfxBuf buf, MRect clip, GfxColor color);

BFont *bfont_builtin(void);
