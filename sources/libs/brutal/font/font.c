#include <brutal/font/builtin.h>
#include <brutal/font/font.h>

float bfont_advance(BFont *font, Rune rune)
{
    switch (font->type)
    {
    case BFONT_BUILTIN:
        return bfont_builtin_advance(rune);

    default:
        panic$("Unknow font type {}!", font->type);
    }
}

BFontMetrics bfont_metrics(BFont *font)
{
    switch (font->type)
    {
    case BFONT_BUILTIN:
        return bfont_builtin_metrics();

    default:
        panic$("Unknow font type {}!", font->type);
    }
}

BFontMesure bfont_mesure(BFont *font, Str str)
{
    float len = 0;

    Rune rune;
    while (uft8_next_rune(&str, &rune))
    {
        len += bfont_advance(font, rune);
    }

    BFontMetrics metrics = bfont_metrics(font);

    return (BFontMesure){
        .bound = m_rect(0, 0, len, metrics.ascend + metrics.descend),
        .baseline = m_vec2(0, metrics.ascend),
    };
}

void bfont_render_rune(BFont *font, Rune rune, MVec2 origin, GfxBuf buf, MRect clip, GfxColor color)
{
    switch (font->type)
    {
    case BFONT_BUILTIN:
        return bfont_builtin_render_rune(rune, origin, buf, clip, color);

    default:
        panic$("Unknow font type {}!", font->type);
    }
}

void bfont_render_str(BFont *font, Str str, MVec2 origin, GfxBuf buf, MRect clip, GfxColor color)
{
    Rune rune;
    while (uft8_next_rune(&str, &rune))
    {
        bfont_render_rune(font, rune, origin, buf, clip, color);
        origin = m_vec2_add(origin, m_vec2(bfont_advance(font, rune), 0));
    }
}

BFont *bfont_builtin(void)
{
    static BFont builtin_font = (BFont){
        .type = BFONT_BUILTIN,
    };
    return &builtin_font;
}
