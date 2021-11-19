#include <brutal/app.h>
#include <brutal/ui.h>

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;

    App app;
    app_init(&app);

    UiSurface ui_surface;
    ui_surface_init(&ui_surface, 800, 600);

    ui_surface_visible(&ui_surface, true);

    GfxSurface gfx_surface = ui_surface_surface(&ui_surface);

    gfx_surface_store(gfx_surface, 16, 16, (GfxColor){0x00, 0x66, 0xff, 0xff});

    ui_surface_flip(&ui_surface, (Recti){0, 0, 32, 32});

    return app_run(&app);
}
