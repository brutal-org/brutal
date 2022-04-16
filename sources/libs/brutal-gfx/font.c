#include <brutal-text>
#include <codec-ssfn2>

#include "font.h"
#include "gfx.h"

GfxFontMetrics gfx_font_metrics(GfxFont font, GfxFontStyle style)
{
    return font.metrics(font.ctx, style);
}

float gfx_font_advance(GfxFont font, GfxFontStyle style, Rune rune)
{
    return font.advance(font.ctx, style, rune);
}

GfxFontMeasure gfx_font_measure(GfxFont font, GfxFontStyle style, Str str)
{
    float len = 0;

    Rune rune;
    while (uft8_next_rune(&str, &rune))
    {
        len += gfx_font_advance(font, style, rune);
    }

    GfxFontMetrics metrics = gfx_font_metrics(font, style);

    return (GfxFontMeasure){
        .capbound = m_rectf(0, 0, len, metrics.ascend + metrics.descend),
        .linebound = m_rectf(0, 0, len, metrics.line_ascend + metrics.line_descend),
        .baseline = m_vec2f(0, metrics.ascend),
    };
}

void gfx_font_render_rune(GfxFont font, GfxFontStyle style, struct _Gfx *gfx, MVec2f baseline, Rune rune)
{
    font.render(font.ctx, style, gfx, baseline, rune);
}

void gfx_font_render_str(GfxFont font, GfxFontStyle style, struct _Gfx *gfx, MVec2f baseline, Str str)
{
    Rune rune;
    while (uft8_next_rune(&str, &rune))
    {
        gfx_font_render_rune(font, style, gfx, baseline, rune);
        baseline = m_vec2f_add(baseline, m_vec2f(gfx_font_advance(font, style, rune), 0));
    }
}

/* --- Builtin Font --------------------------------------------------------- */

GfxFontMetrics gfx_font_builtin_metrics(MAYBE_UNUSED void *ctx, GfxFontStyle style)
{
    return (GfxFontMetrics){
        .line_ascend = 12 * style.scale,
        .ascend = 10 * style.scale,
        .captop = 10 * style.scale,
        .descend = 3 * style.scale,
        .line_descend = 4 * style.scale,

        .advance = GFX_FONT_BUILTIN_WIDTH * style.scale,
    };
}

float gfx_font_builtin_advance(MAYBE_UNUSED void *ctx, MAYBE_UNUSED GfxFontStyle style, MAYBE_UNUSED Rune rune)
{
    return GFX_FONT_BUILTIN_WIDTH * style.scale;
}

void gfx_font_builtin_render(MAYBE_UNUSED void *ctx, GfxFontStyle style, Gfx *gfx, MVec2f baseline, Rune rune)
{
    baseline = m_vec2f_add(gfx_peek(gfx)->origin, baseline);

    GfxFontMetrics metrics = gfx_font_builtin_metrics(ctx, style);
    uint8_t index = cp437_from_rune(rune);
    GfxColor color = gfx_paint_sample(gfx_peek(gfx)->fill, 0, 0);

    for (int y = 0; y < GFX_FONT_BUILTIN_HEIGHT * style.scale; y++)
    {
        for (int x = 0; x < GFX_FONT_BUILTIN_WIDTH * style.scale; x++)
        {
            int xx = (GFX_FONT_BUILTIN_WIDTH - (x / style.scale) - 1);
            int yy = (int)(index * GFX_FONT_BUILTIN_HEIGHT + (y / style.scale));

            uint8_t byte = gfx_font_builtin_data[yy];
            uint8_t bit = (byte >> xx) & 0b1;

            MVec2f pos = m_vec2f_add(baseline, m_vec2f(x, y - (metrics.ascend)));

            if (bit && m_rectf_collide_point(gfx_peek(gfx)->clip, pos))
            {
                gfx_buf_blend(gfx->buf, pos.x, pos.y, color);
            }
        }
    }
}

GfxFont gfx_font_builtin(void)
{
    return (GfxFont){
        .ctx = nullptr,
        .metrics = gfx_font_builtin_metrics,
        .advance = gfx_font_builtin_advance,
        .render = gfx_font_builtin_render,
    };
}
