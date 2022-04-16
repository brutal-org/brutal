#include <brutal-gfx>
#include <brutal-test>
#include <brutal-ui>

static char const *_logo = "M86 86L43 43L0 0H43H86V43V86ZM43 43L43 0L86 43H43ZM129 86C105.252 86 86 66.7482 86 43C86 19.2518 105.252 0 129 0C152.748 0 172 19.2518 172 43C172 66.7482 152.748 86 129 86ZM107.5 43C107.5 54.8741 117.126 64.5 129 64.5C140.874 64.5 150.5 54.8741 150.5 43C150.5 31.1259 140.874 21.5 129 21.5C117.126 21.5 107.5 31.1259 107.5 43ZM86 129C86 137.505 88.5219 145.818 93.2468 152.89C97.9717 159.961 104.687 165.472 112.545 168.727C120.402 171.981 129.048 172.833 137.389 171.174C145.73 169.515 153.392 165.419 159.406 159.406C165.419 153.392 169.515 145.73 171.174 137.389C172.833 129.048 171.981 120.402 168.727 112.545C165.472 104.687 159.961 97.9717 152.89 93.2468C145.818 88.5219 137.505 86 129 86H86H43V43L0 43L43 86H0V172H86V129ZM86 129H129V86L86 129ZM21.5 150.5V107.5H64.5V150.5H21.5Z";

test$(gfx_fill_path)
{
    gfx_origin(test_use_gfx(), m_vec2f(64, 64));
    gfx_fill_style(test_use_gfx(), gfx_paint_fill(GFX_WHITE));
    gfx_fill_svg(test_use_gfx(), str$(_logo), GFX_FILL_EVENODD);
}

test$(gfx_fill_path_rotated)
{
    MTrans2 trans = m_trans2_rotate(1 / 16.0);
    gfx_origin(test_use_gfx(), m_vec2f(500, 300));
    gfx_trans(test_use_gfx(), trans);

    gfx_fill_style(test_use_gfx(), gfx_paint_fill(GFX_WHITE));
    gfx_fill_svg(test_use_gfx(), str$(_logo), GFX_FILL_EVENODD);
}

test$(gfx_stroke_path)
{
    gfx_origin(test_use_gfx(), m_vec2f(64, 64));
    gfx_fill_style(test_use_gfx(), gfx_paint_fill(GFX_WHITE));
    gfx_stroke_svg(test_use_gfx(), str$(_logo));
}
