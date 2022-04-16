#include <brutal-gfx>
#include <brutal-test>
#include <brutal-ui>

test$(gfx_fill_rect)
{
    gfx_fill_style(test_use_gfx(), gfx_paint_fill(UI_COLOR_BASE02));

    gfx_fill_rect(test_use_gfx(), m_rectf(10, 10, 512, 50), 0);
}

test$(gfx_fill_rect_rounded)
{
    gfx_fill_style(test_use_gfx(), gfx_paint_fill(UI_COLOR_BASE02));

    gfx_fill_rect(test_use_gfx(), m_rectf(10, 10, 512, 50), 32);
}

test$(gfx_fill_rect_gradient)
{
    GfxGradient grad = {
        .len = 5,
        .stops = {
            {
                .loc = 0.0f,
                .color = UI_COLOR_LIGHT_BLUE,
            },
            {
                .loc = 0.25f,
                .color = UI_COLOR_LIGHT_RED,
            },
            {
                .loc = 0.5f,
                .color = UI_COLOR_BASE08,
            },
            {
                .loc = 0.75f,
                .color = UI_COLOR_LIGHT_RED,
            },
            {
                .loc = 1.f,
                .color = UI_COLOR_LIGHT_BLUE,
            },
        },

    };

    gfx_fill_style(test_use_gfx(), gfx_paint_gradient(&grad));
    gfx_fill_rect(test_use_gfx(), m_rectf(10, 10, 512, 512), 32);
}
