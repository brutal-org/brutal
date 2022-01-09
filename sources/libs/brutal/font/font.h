#pragma once

#include <brutal/font/metrics.h>
#include <brutal/gfx/buf.h>
#include <brutal/text/rune.h>

typedef enum
{
    BFONT_BUILTIN,
} BFontType;

typedef struct
{
    BFontType type;
} BFont;

float bfont_advance(BFont *font, Rune rune, float scale);

BFontMetrics bfont_metrics(BFont *font, float scale);

BFontMesure bfont_mesure(BFont *font, Str str, float scale);

void bfont_render_rune(BFont *font, Rune rune, MVec2 baseline, GfxBuf buf, MRect clip, float scale, GfxColor color);

void bfont_render_str(BFont *font, Str str, MVec2 baseline, GfxBuf buf, MRect clip, float scale, GfxColor color);

BFont *bfont_builtin(void);
