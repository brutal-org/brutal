#include <brutal/ui.h>

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;

    UiApp app;
    ui_app_init(&app);

    UiWin win;
    ui_win_init(&win, 800, 600);

    ui_win_visible(&win, true);

    GfxSurface gfx_surface = ui_win_gfx(&win);

    gfx_surface_store(gfx_surface, 16, 16, (GfxColor){0x00, 0x66, 0xff, 0xff});

    ui_win_flip(&win, (Recti){0, 0, 32, 32});
    return ui_app_run(&app);
}
