#pragma once

#include <brutal-io>
#include <brutal-math>
#include <brutal-text>

struct _Gfx;

/**

                                                            <- line_ascend
       _____       _             _                          <- ascend
      / ____|     | |           | |                         <- captop
     | |  __ _   _| |_ ___ _ __ | |__   ___ _ __ __ _
     | | |_ | | | | __/ _ \ '_ \| '_ \ / _ \ '__/ _` |
     | |__| | |_| | ||  __/ | | | |_) |  __/ | | (_| |
      \_____|\__,_|\__\___|_| |_|_.__/ \___|_|  \__, |      <- baseline (origin)
                                                 __/ |
                                                |___/       <- descend
     | ---- |                                               ...advance
                                                            <- line_descend

*/
typedef struct
{
    float line_ascend;
    float ascend;
    float captop;
    float descend;
    float line_descend;

    float advance;
} GfxFontMetrics;

typedef struct
{
    MRectf capbound;
    MRectf linebound;
    MVec2f baseline;
} GfxFontMeasure;

// https://docs.microsoft.com/en-us/typography/opentype/otspec180/fvar
// https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_Fonts/Variable_Fonts_Guide
typedef struct
{
    /// Used to vary the size of the text
    float scale;

    /// Used to vary stroke thicknesses to give variation from lighter to blacker.
#define GFX_FONT_THIN 0.1f
#define GFX_FONT_EXTRA_LIGHT 0.2f
#define GFX_FONT_LIGHT 0.3f
#define GFX_FONT_REGULAR 0.4f
#define GFX_FONT_MEDIUM 0.5f
#define GFX_FONT_SEMI_BOLD 0.6f
#define GFX_FONT_BOLD 0.7f
#define GFX_FONT_EXTRA_BOLD 0.8f
#define GFX_FONT_BLACK 0.9f
    float weight;

    /// Used to vary width of text from narrower to wider.
    float width;

    /// Used to vary between upright and slanted text.
    float slant;

    /// Used to vary between non-italic and italic.
    bool italic;

    /// Used to vary between monospace and proportional.
    bool monospace;
} GfxFontStyle;

#define GFX_FONT_DEFAULT            \
    (GfxFontStyle)                  \
    {                               \
        .scale = 1.0f,              \
        .weight = GFX_FONT_REGULAR, \
        .width = 1.0f,              \
        .slant = 0.0f,              \
        .italic = false,            \
    }

typedef GfxFontMetrics GfxFontMetricsFn(void *ctx, GfxFontStyle style);
typedef float GfxFontAdvanceFn(void *ctx, GfxFontStyle style, Rune rune);
typedef void GfxFontRenderFn(void *ctx, GfxFontStyle style, struct _Gfx *gfx, MVec2f baseline, Rune rune);

typedef struct
{
    void *ctx;

    GfxFontMetricsFn *metrics;
    GfxFontAdvanceFn *advance;
    GfxFontRenderFn *render;
} GfxFont;

GfxFontMetrics gfx_font_metrics(GfxFont font, GfxFontStyle style);

float gfx_font_advance(GfxFont font, GfxFontStyle style, Rune rune);

GfxFontMeasure gfx_font_measure(GfxFont font, GfxFontStyle style, Str str);

void gfx_font_render_rune(GfxFont font, GfxFontStyle style, struct _Gfx *gfx, MVec2f baseline, Rune rune);

void gfx_font_render_str(GfxFont font, GfxFontStyle style, struct _Gfx *gfx, MVec2f baseline, Str str);

/* --- Builtin Font --------------------------------------------------------- */

#define GFX_FONT_BUILTIN_WIDTH (8)
#define GFX_FONT_BUILTIN_HEIGHT (16)

extern uint8_t gfx_font_builtin_data[256 * 16];

GfxFont gfx_font_builtin(void);
