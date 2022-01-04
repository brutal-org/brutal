#include <brutal/alloc.h>
#include <brutal/io.h>
#include <brutal/ui.h>

UiWin *demo_win_create(UiApp *app)
{
    UiWin *self = ui_win_create(app, m_rect(0, 0, 800, 600), 0);

    UiView *container = ui_panel_create(GFX_DARK_GRAY);
    ui_view_style(
        container,
        (UiStyle){
            .layout = UI_LAYOUT_DOCK,
            .flow = M_FLOW_LEFT_TO_RIGHT,
        });

    UiView *toolbar = ui_panel_create(GFX_GRAY);
    ui_view_style(
        toolbar,
        (UiStyle){
            .layout = UI_LAYOUT_DOCK,
            .dock = UI_DOCK_TOP,
            .padding = (UiSpacing){6, 6, 6, 6},
            .size.max.height = 48,
        });

    ui_view_mount(container, toolbar);

    for (int i = 0; i < 5; i++)
    {
        UiView *item = ui_panel_create(GFX_DIM_GRAY);
        ui_view_style(
            item,
            (UiStyle){
                .dock = UI_DOCK_START,
                .margin.start = 4,
                .size.square = true,
            });

        ui_view_mount(toolbar, item);
    }

    ui_win_mount(self, container);

    UiView *button = ui_button_create_with_text(str$("HELLO WORLD!"));
    ui_view_style(
        button,
        (UiStyle){
            .margin.start = 4,
            .dock = UI_DOCK_START,
            .size.max.width = 160,
        });

    ui_view_mount(toolbar, button);

    UiView *button1 = ui_button_create_with_text(str$("HELLO FRIENDS!"));
    ui_view_style(
        button1,
        (UiStyle){
            .margin.start = 4,
            .dock = UI_DOCK_START,
            .size.max.width = 160,
        });

    ui_view_mount(toolbar, button1);

    UiView *icon1 = ui_icon_create();
    ui_view_style(
        icon1,
        (UiStyle){
            .dock = UI_DOCK_FILL,
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
