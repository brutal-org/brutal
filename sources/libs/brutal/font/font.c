#include <brutal/font/builtin.h>
#include <brutal/font/font.h>
#include <brutal/text/utf8.h>

float bfont_advance(BFont *font, Rune rune, float scale)
{
    switch (font->type)
    {
    case BFONT_BUILTIN:
        return bfont_builtin_advance(rune, scale);

    default:
        panic$("Unknow font type {}!", font->type);
    }
}

BFontMetrics bfont_metrics(BFont *font, float scale)
{
    switch (font->type)
    {
    case BFONT_BUILTIN:
        return bfont_builtin_metrics(scale);

    default:
        panic$("Unknow font type {}!", font->type);
    }
}

BFontMesure bfont_mesure(BFont *font, Str str, float scale)
{
    float len = 0;

    Rune rune;
    while (uft8_next_rune(&str, &rune))
    {
        len += bfont_advance(font, rune, scale);
    }

    BFontMetrics metrics = bfont_metrics(font, scale);

    return (BFontMesure){
        .capbound = m_rect(0, 0, len, metrics.ascend + metrics.descend),
        .linebound = m_rect(0, 0, len, metrics.line_ascend + metrics.line_descend),
        .baseline = m_vec2(0, metrics.ascend * scale),
    };
}

void bfont_render_rune(BFont *font, Rune rune, MVec2 origin, GfxBuf buf, MRect clip, float scale, GfxColor color)
{
    switch (font->type)
    {
    case BFONT_BUILTIN:
        return bfont_builtin_render_rune(rune, origin, buf, clip, scale, color);

    default:
        panic$("Unknow font type {}!", font->type);
    }
}

void bfont_render_str(BFont *font, Str str, MVec2 origin, GfxBuf buf, MRect clip, float scale, GfxColor color)
{
    Rune rune;
    while (uft8_next_rune(&str, &rune))
    {
        bfont_render_rune(font, rune, origin, buf, clip, scale, color);
        origin = m_vec2_add(origin, m_vec2(bfont_advance(font, rune, scale), 0));
    }
}

BFont *bfont_builtin(void)
{
    static BFont builtin_font = (BFont){
        .type = BFONT_BUILTIN,
    };
    return &builtin_font;
}
