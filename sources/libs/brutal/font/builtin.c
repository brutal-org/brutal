#include <brutal/font/builtin.h>
#include <brutal/text/cp437.h>

float bfont_builtin_advance(Rune rune, float scale)
{
    UNUSED(rune);
    return BFONT_BUILTIN_WIDTH * scale;
}

BFontMetrics bfont_builtin_metrics(float scale)
{
    return (BFontMetrics){
        .line_ascend = 12 * scale,
        .ascend = 10 * scale,
        .captop = 10 * scale,
        .descend = 3 * scale,
        .line_descend = 4 * scale,

        .advance = BFONT_BUILTIN_WIDTH * scale,
    };
}

void bfont_builtin_render_rune(Rune rune, MVec2 origin, GfxBuf buf, MRect clip, float scale, GfxColor color)
{
    uint8_t index = cp437_from_rune(rune);

    for (int y = 0; y < BFONT_BUILTIN_HEIGHT * scale; y++)
    {
        for (int x = 0; x < BFONT_BUILTIN_WIDTH * scale; x++)
        {
            int xx = (BFONT_BUILTIN_WIDTH - (x / scale) - 1);
            int yy = (int)(index * BFONT_BUILTIN_HEIGHT + (y / scale));

            uint8_t byte = bfont_builtin_data[yy];
            uint8_t bit = (byte >> xx) & 0b1;

            MVec2 pos = m_vec2_add(origin, m_vec2(x, y - (bfont_builtin_metrics(scale).ascend * scale)));

            if (bit && m_rect_collide_point(clip, pos))
            {
                gfx_buf_blend(buf, pos.x, pos.y, color);
            }
        }
    }
}
