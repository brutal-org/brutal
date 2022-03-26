#include <brutal/ui.h>
#include "../test.h"

GFX_TEST(gfx_basic_text)
{
    gfx_fill_style(test_gfx(), gfx_paint_fill(UI_COLOR_BASE08));

    float scale = 1.f;
    float pos = 64.f;
    for (int i = 0; i < 8; i++)
    {
        gfx_font_style(test_gfx(), (GfxFontStyle){.scale = scale});
        gfx_text(test_gfx(), m_vec2f(18, pos), str$("The quick brown fox jumps over the lazy dog."));
        scale *= 1.25;
        pos += scale * 16;
    }
}
