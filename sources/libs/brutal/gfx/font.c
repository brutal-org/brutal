#include <brutal/codec/ssfn2/ssfn2.h>
#include <brutal/gfx/font.h>
#include <brutal/gfx/gfx.h>
#include <brutal/text/cp437.h>
#include <brutal/text/utf8.h>

GfxFontMetrics gfx_font_metrics(GfxFont font)
{
    return font.metrics(font.ctx, font.style);
}

float gfx_font_advance(GfxFont font, Rune rune)
{
    return font.advance(font.ctx, font.style, rune);
}

GfxFontMesure gfx_font_mesure(GfxFont font, Str str)
{
    float len = 0;

    Rune rune;
    while (uft8_next_rune(&str, &rune))
    {
        len += gfx_font_advance(font, rune);
    }

    GfxFontMetrics metrics = gfx_font_metrics(font);

    return (GfxFontMesure){
        .capbound = m_rect(0, 0, len, metrics.ascend + metrics.descend),
        .linebound = m_rect(0, 0, len, metrics.line_ascend + metrics.line_descend),
        .baseline = m_vec2(0, metrics.ascend),
    };
}

void gfx_font_render_rune(GfxFont font, struct _Gfx *gfx, MVec2 baseline, Rune rune)
{
    font.render(font.ctx, font.style, gfx, baseline, rune);
}

void gfx_font_render_str(GfxFont font, struct _Gfx *gfx, MVec2 baseline, Str str)
{
    Rune rune;
    while (uft8_next_rune(&str, &rune))
    {
        gfx_font_render_rune(font, gfx, baseline, rune);
        baseline = m_vec2_add(baseline, m_vec2(gfx_font_advance(font, rune), 0));
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

void gfx_font_builtin_render(MAYBE_UNUSED void *ctx, GfxFontStyle style, Gfx *gfx, MVec2 baseline, Rune rune)
{
    uint8_t index = cp437_from_rune(rune);

    for (int y = 0; y < GFX_FONT_BUILTIN_HEIGHT * style.scale; y++)
    {
        for (int x = 0; x < GFX_FONT_BUILTIN_WIDTH * style.scale; x++)
        {
            int xx = (GFX_FONT_BUILTIN_WIDTH - (x / style.scale) - 1);
            int yy = (int)(index * GFX_FONT_BUILTIN_HEIGHT + (y / style.scale));

            uint8_t byte = gfx_font_builtin_data[yy];
            uint8_t bit = (byte >> xx) & 0b1;

            MVec2 pos = m_vec2_add(baseline, m_vec2(x, y - (gfx_font_builtin_metrics(ctx, style).ascend * style.scale)));

            if (bit && m_rect_collide_point(gfx_peek(gfx)->clip, pos))
            {
                gfx_buf_blend(gfx->buf, pos.x, pos.y, gfx_peek(gfx)->color);
            }
        }
    }
}

GfxFont gfx_font_builtin(void)
{
    return (GfxFont){
        .ctx = nullptr,
        .style = {
            .scale = 1,
        },
        .metrics = gfx_font_builtin_metrics,
        .advance = gfx_font_builtin_advance,
        .render = gfx_font_builtin_render,
    };
}

/* --- SSFN2 Font --------------------------------------------------------- */

GfxFontMetrics gfx_font_ssfn2_metrics(void *ctx, GfxFontStyle style)
{
    SSFN2Font *font = (SSFN2Font *)ctx;

    return (GfxFontMetrics){
        .line_ascend = 12 * style.scale,
        .ascend = 10 * style.scale,
        .captop = 10 * style.scale,
        .descend = 3 * style.scale,
        .line_descend = 4 * style.scale,

        .advance = load_le(font->header.width) * style.scale,
    };
}

float gfx_font_ssfn2_advance(MAYBE_UNUSED void *ctx, MAYBE_UNUSED GfxFontStyle style, MAYBE_UNUSED Rune rune)
{
    SSFN2Font *font = (SSFN2Font *)ctx;
    return font->glyphs[rune].width;
}

void gfx_font_ssfn2_render(MAYBE_UNUSED void *ctx, MAYBE_UNUSED GfxFontStyle style, Gfx *gfx, MVec2 baseline, Rune rune)
{
    gfx_push(gfx);
    SSFN2Font *font = (SSFN2Font *)ctx;
    SSFN2Glyph glyph = font->glyphs[rune];
    gfx_origin(gfx, baseline);
    gfx_stroke_path(gfx, &glyph.path);

    gfx_pop(gfx);
}

GfxFont gfx_font_ssfn2(struct SSFN2Font *font)
{
    return (GfxFont){
        .ctx = font,
        .style = {
            .scale = 0.01,
        },
        .metrics = gfx_font_ssfn2_metrics,
        .advance = gfx_font_ssfn2_advance,
        .render = gfx_font_ssfn2_render,
    };
}
