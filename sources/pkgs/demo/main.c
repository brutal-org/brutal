#include <brutal/alloc.h>
#include <brutal/io.h>
#include <brutal/ui.h>

UiWin *demo_win_create(UiApp *app)
{
    UiWin *self = ui_win_create(app, m_rect(0, 0, 800, 600), UI_WIN_NORMAL);

    UiView *container = ui_panel_create(GFX_DARK_GRAY);

    UiView *toolbar = ui_panel_create(GFX_GRAY);
    ui_view_layout(
        toolbar,
        (UiLayout){
            .dock = M_DOCK_TOP,
            .padding = (MSpacing){6, 6, 6, 6},
            .size.max.height = 48,
        });

    ui_view_mount(container, toolbar);

    for (int i = 0; i < 5; i++)
    {
        UiView *item = ui_panel_create(GFX_DIM_GRAY);
        ui_view_layout(
            item,
            (UiLayout){
                .dock = M_DOCK_START,
                .margin.start = 4,
                .size.min.width = 64,
            });

        ui_view_mount(toolbar, item);
    }

    ui_win_mount(self, container);

    UiView *button = ui_button_create_with_text(str$("HELLO WORLD!"));
    ui_view_layout(
        button,
        (UiLayout){
            .margin.start = 4,
            .dock = M_DOCK_START,
            .size.max.width = 160,
        });

    ui_view_mount(toolbar, button);

    UiView *button1 = ui_button_create_with_text(str$("HELLO FRIENDS!"));
    ui_view_layout(
        button1,
        (UiLayout){
            .margin.start = 4,
            .dock = M_DOCK_START,
            .size.max.width = 160,
        });

    ui_view_mount(toolbar, button1);

    UiView *icon1 = ui_icon_create();
    ui_view_layout(
        icon1,
        (UiLayout){
            .dock = M_DOCK_FILL,
        });
    ui_view_mount(container, icon1);

    return self;
}

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;

    UiApp app;
    ui_app_init(&app);

    UiWin *win = demo_win_create(&app);
    ui_win_show(win);

    int result = ui_app_run(&app);

    ui_win_deref(win);
    ui_app_deinit(&app);

    return result;
}
