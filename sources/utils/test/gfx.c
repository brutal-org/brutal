#include <brutal/time.h>
#include <brutal/ui.h>
#include "test.h"

static UiApp app;
static UiWin *win;
static Gfx *cur_gfx;

static void test_gfx_draw_fps(Gfx *gfx, double avg_delta)
{
    Str v = str_fmt$(test_alloc(), "fps: {} (dt: {})", 1.f / avg_delta, avg_delta);
    gfx_fill_style(gfx, gfx_paint_fill(GFX_WHITE));
    gfx_text(gfx, m_vec2f(0, 20), v);
    alloc_free(test_alloc(), (void *)v.buf);
}

Gfx *test_gfx(void)
{
    return cur_gfx;
}

static void gfx_test_init(void)
{
    ui_app_init(&app);
    win = ui_win_create(&app, m_rectf(0, 0, 1080, 720), UI_WIN_NORMAL);

    ui_win_show(win);
    cur_gfx = &win->gfx;
}

static void gfx_test_deinit(void)
{
    ui_win_deref(win);
    ui_app_deinit(&app);
}

static bool gfx_test_should_continue(Test test, long frame, NanoSeconds begin, long default_test_runtime)
{
    if (!app.alive)
    {
        return false;
    }

    if (test.flags & TEST_COUNTED)
    {
        return frame < test.time_count;
    }
    else
    {
        return begin + (default_test_runtime * 1000000000.) > time_ns_now();
    }
}

double test_gfx_run(Test test, long default_test_runtime)
{
    gfx_test_init();
    gfx_begin(&win->gfx, ui_win_gfx(win));
    gfx_font_style(&win->gfx, UI_FONT_BODY);
    gfx_font_family(&win->gfx, win->app->font);
    gfx_push(&win->gfx);

    // only used to know when we need to stop the benchmark, this hasn't any implication in the calculation of the framerate/dt
    NanoSeconds time_begin = time_ns_now();

    double avg_delta = 0;
    long frame = 0;
    while (gfx_test_should_continue(test, frame, time_begin, default_test_runtime))
    {
        gfx_clear(&win->gfx, GFX_BLACK);

        // here we calculate the time it tooks to render the benchmark
        gfx_push(&win->gfx);
        NanoSeconds start = time_ns_now();

        test.func();

        NanoSeconds end = time_ns_now();
        gfx_pop(&win->gfx);

        // add the delta time to to the average: (last_avg * n + new_val ) / (n + 1)
        avg_delta = (avg_delta * frame + (end - start) / 1000000000.) / (frame + 1);

        test_gfx_draw_fps(&win->gfx, avg_delta);
        ui_win_flip(win, ui_win_bound(win));
        ui_app_pump(&app);

        frame++;
        _test_set_iteration(frame);
    }

    gfx_pop(&win->gfx);
    gfx_end(&win->gfx);
    gfx_test_deinit();

    return avg_delta;
}
