#pragma once

#include <brutal/font/metrics.h>
#include <brutal/gfx/buf.h>
#include <brutal/text/rune.h>

#define BFONT_BUILTIN_WIDTH (8)
#define BFONT_BUILTIN_HEIGHT (16)

extern uint8_t bfont_builtin_data[256 * 16];

float bfont_builtin_advance(Rune str, float scale);

BFontMetrics bfont_builtin_metrics(float scale);

void bfont_builtin_render_rune(Rune rune, MVec2 origin, GfxBuf buf, MRect clip, float scale, GfxColor color);
