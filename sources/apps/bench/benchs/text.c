#include <brutal/ui.h>
#include "../bench.h"

BENCH(basic_text)
{
    gfx_fill_style(gfx, gfx_paint_fill(UI_COLOR_BASE08));
    gfx_text(gfx, m_vec2f(64, 64), str$("abcdefghijklmnopqrstuvwxyz \n brutal is love brutal is live"));
}
