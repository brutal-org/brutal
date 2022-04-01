#include <bruwutal/ui.h>
#include "../bench.h"

BENCH(basic_text)
{
    gfx_fill_style(gfx, gfx_paint_fill(UI_COLOR_BASE08));

    float scale = 1.f;
    float pos = 64.f;
    for (int i = 0; i < 8; i++)
    {
        gfx_font_style(gfx, (GfxFontStyle){.scale = scale});
        gfx_text(gfx, m_vec2f(18, pos), str$("The quick brown fox jumps over the lazy dog."));
        scale *= 1.25;
        pos += scale * 16;
    }
}
