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
            .flow = M_FLOW_TOP_TO_BOTTOM,
        });

    UiView *toolbar = ui_panel_create(GFX_BLUE);
    ui_view_style(
        toolbar,
        (UiStyle){
            .layout = UI_LAYOUT_DOCK,
            .dock = UI_DOCK_TOP,
            .padding = (UiSpacing){6, 6, 6, 6},
            .size.max.height = 48,
        });

    ui_view_mount(container, toolbar);

    UiView *footer = ui_panel_create(GFX_GREEN);
    ui_view_style(
        footer,
        (UiStyle){
            .layout = UI_LAYOUT_DOCK,
            .dock = UI_DOCK_BOTTOM,
            .padding = (UiSpacing){6, 6, 6, 6},
            .size.max.height = 48,
        });

    ui_view_mount(container, footer);

    UiView *right_sidebar = ui_panel_create(GFX_RED);
    ui_view_style(
        right_sidebar,
        (UiStyle){
            .layout = UI_LAYOUT_DOCK,
            .dock = UI_DOCK_END,
            .padding = (UiSpacing){6, 6, 6, 6},
            .size.max.width = 48,
        });

    ui_view_mount(container, right_sidebar);

    UiView *left_sidebar = ui_panel_create(GFX_YELLOW);
    ui_view_style(
        left_sidebar,
        (UiStyle){
            .layout = UI_LAYOUT_DOCK,
            .dock = UI_DOCK_START,
            .padding = (UiSpacing){6, 6, 6, 6},
            .size.max.width = 48,
        });

    ui_view_mount(container, left_sidebar);

    UiView *test = ui_panel_create(GFX_WHITE);
    ui_view_style(
        test,
        (UiStyle){
            .layout = UI_LAYOUT_DOCK,
            .dock = UI_DOCK_FILL,
            .padding = (UiSpacing){6, 6, 6, 6},
        });

    ui_view_mount(container, test);

    for (int i = 0; i < 5; i++)
    {
        UiView *item = ui_panel_create(GFX_DARK_MAGENTA);
        ui_view_style(
            item,
            (UiStyle){
                .dock = UI_DOCK_END,
                .margin.start = 4,
                .size.square = true,
            });

        ui_view_mount(toolbar, item);
    }

    ui_win_mount(self, container);

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
    Emit emit;
    emit_init(&emit, io_chan_out());
    ui_view_dump(win->root, &emit);
    return ui_app_run(&app);
}
