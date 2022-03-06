#include <brutal/alloc.h>
#include <brutal/io.h>
#include <brutal/parse.h>
#include <brutal/time.h>
#include <brutal/ui.h>
#include <embed/win.h>
#include "bench.h"

// frames pack: the number of frames to pack into a single tick count:
// low frames pack are not really good when you do fast benchmark.

static size_t _len = 0;
static Bench _benchs[1024] = {};

void bench_register(Bench test)
{
    _benchs[_len] = test;
    _len++;
}

BENCH(empty)
{
    // do nothing
}

static void bench_run_tick(Gfx *self, Bench bench, int tick)
{
    bench.func(self, tick);
}

static void bench_draw_fps(Gfx *gfx, double avg_delta)
{
    Str v = str_fmt(alloc_global(), "fps: {} (dt: {})", 1.f / avg_delta, avg_delta);
    gfx_fill_style(gfx, gfx_paint_fill(GFX_WHITE));
    gfx_text(gfx, m_vec2f(0, 20), v);
    alloc_free(alloc_global(), (void *)v.buf);
}

static float bench_run(UiApp *app, UiWin* win, Bench bench, int sec_per_bench)
{
    gfx_begin(&win->gfx, ui_win_gfx(win));
    gfx_clip(&win->gfx, ui_win_bound(win));
    gfx_font_style(&win->gfx, UI_FONT_BODY);
    gfx_font_family(&win->gfx, win->app->font);
    gfx_push(&win->gfx);

    // only used to know when we need to stop the benchmark, this hasn't any implication in the calculation of the framerate/dt
    TimeStamp time_begin = timestamp_now();

    double avg_delta = 0;
    long frame = 0;

    while (time_begin + sec_per_bench > timestamp_now() && app->alive)
    {
        gfx_clear(&win->gfx, GFX_BLACK);

        // here we calculate the time it tooks to render the benchmark
        NanoSeconds start = time_ns_now();

        bench_run_tick(&win->gfx, bench, frame);

        NanoSeconds end = time_ns_now();

        double delta_s = (end - start) / (double)(1000000000.f);

        // add delta_s to to the average delta time
        avg_delta = ((avg_delta * frame) + delta_s) / (frame + 1.f);

        bench_draw_fps(&win->gfx, avg_delta);
        embed_win_flip(win, ui_win_bound(win));

        frame++;

        ui_app_pump(app);

    }

    gfx_pop(&win->gfx);
    gfx_end(&win->gfx);

    log$("'{}': fps: {} / dt: {}s for {} frames", bench.name, 1 / (avg_delta), avg_delta, frame );
    return avg_delta;
}

static int bench_run_by_pattern(Str pattern, UiApp* app, UiWin *win, int sec_per_bench)
{
    for (size_t i = 0; i < _len; i++)
    {
        if (glob_match_str(pattern, str$(_benchs[i].name)))
        {
            bench_run(app, win, _benchs[i], sec_per_bench);
        }
    }

    return 0;
}

static int bench_run_all(UiApp* app, UiWin *win, int sec_count)
{
    for (size_t i = 0; i < _len; i++)
    {
        bench_run(app, win, _benchs[i], sec_count);
    }

    return 0;
}

int main(int argc, char const *argv[])
{
    long sec_per_bench = 3;
    Str test_to_run = str$("");

    // FIXME: implement a good way to parse arguments in the brutal library

    for (int i = 1; i < argc; i++)
    {
        if (str_eq(str$(argv[i]), str$("-t")))
        {
            assert_lower_than(i + 1, argc);
            test_to_run = str$(argv[i + 1]);
            i++;
        }
        else if (str_eq(str$(argv[i]), str$("-l")))
        {
            log$("dumping different bench:");
            for (size_t y = 0; y < _len; y++)
            {
                log$("- {}", _benchs[y].name);
            }
            return 0;
        }
        else if (str_eq(str$(argv[i]), str$("-s")))
        {
            assert_lower_than(i + 1, argc);
            sec_per_bench = 0;
            str_to_int(str$(argv[i + 1]), &sec_per_bench);
            i++;
        }
    }

    UiApp app;
    ui_app_init(&app);
    UiWin *win = ui_win_create(&app, m_rectf(0, 0, 1080, 720), UI_WIN_NORMAL);

    ui_win_show(win);

    if (str_any(test_to_run))
    {
        bench_run_by_pattern(test_to_run, &app, win, sec_per_bench);
    }
    else
    {
        bench_run_all(&app, win, sec_per_bench);
    }

    ui_win_deref(win);
    ui_app_deinit(&app);

    return 0;
}
