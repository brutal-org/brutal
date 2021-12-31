#include <brutal/font/builtin.h>

float bfont_builtin_advance(Rune rune)
{
    UNUSED(rune);
    return BFONT_BUILTIN_WIDTH;
}

BFontMetrics bfont_builtin_metrics(void)
{
    return (BFontMetrics){
        .line_ascend = 12,
        .ascend = 10,
        .captop = 10,
        .descend = 3,
        .line_descend = 4,

        .advance = BFONT_BUILTIN_WIDTH,
    };
}

void bfont_builtin_render_rune(Rune rune, MVec2 origin, GfxBuf buf, MRect clip, GfxColor color)
{
    uint8_t index = cp437_from_rune(rune);

    for (int y = 0; y < BFONT_BUILTIN_HEIGHT; y++)
    {
        for (int x = 0; x < BFONT_BUILTIN_WIDTH; x++)
        {
            uint8_t byte = bfont_builtin_data[index * BFONT_BUILTIN_HEIGHT + y];
            uint8_t bit = (byte >> (BFONT_BUILTIN_WIDTH - x - 1)) & 0b1;
            MVec2 pos = m_vec2_add(origin, m_vec2(x, y - bfont_builtin_metrics().ascend));

            if (bit && m_rect_collide_point(clip, pos))
            {
                gfx_buf_blend(buf, pos.x, pos.y, color);
            }
        }
    }
}
