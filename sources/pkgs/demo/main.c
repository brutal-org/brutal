#include <brutal/alloc.h>
#include <brutal/ui.h>

typedef struct demo_win
{
    UI_WIN_BASE(struct demo_win);

    int x, y;
} DemoWin;

void demo_win_paint(DemoWin *win, Gfx *gfx)
{
    gfx_clear(gfx, GFX_BLACK);

    gfx_fill(
        gfx,
        gfx_paint_gradient((GfxGradient){
            .stops = {
                {0.0, GFX_LIGHT_BLUE},
                {0.2, GFX_LIGHT_BLUE},
                {0.2, GFX_PINK},
                {0.4, GFX_PINK},
                {0.4, GFX_WHITE},
                {0.6, GFX_WHITE},
                {0.6, GFX_PINK},
                {0.8, GFX_PINK},
                {0.8, GFX_LIGHT_BLUE},
                {1.0, GFX_LIGHT_BLUE},
            },
            .len = 10,
        }));

    gfx_poly(
        gfx,
        (Vec2[]){
            {win->x, win->y},
            {win->x + 128, win->y + 128},
            {win->x - 128, win->y + 128},
        },
        3);

    win->x++;
    win->y++;
}

void demo_win_event(MAYBE_UNUSED DemoWin *win, MAYBE_UNUSED UiEvent *event)
{
    log$("EVENT");
}

UiWin *demo_win_create(UiApp *app, Alloc *alloc)
{
    DemoWin *self = alloc_make(alloc, DemoWin);

    self->paint = demo_win_paint;
    self->event = demo_win_event;

    ui_win_init(base$(self), app, (Rect){0, 0, 800, 600}, UI_WIN_ANIMATED);
    return base$(self);
}

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;

    UiApp app;
    ui_app_init(&app);

    UiWin *win = demo_win_create(&app, alloc_global());
    ui_win_show(win);

    return ui_app_run(&app);
}
