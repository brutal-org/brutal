#include <brutal-io>
#include <brutal-ui>

UiWin *demo_win_create(UiApp *app)
{
    UiWin *self = ui_win_create(app, m_rectf(0, 0, 800, 600), UI_WIN_NORMAL);

    UiView *container = ui_panel_create(GFX_DARK_GRAY);

    UiView *toolbar = ui_panel_create(GFX_GRAY);
    ui_view_layout(toolbar, "dock-top p-6 max-h-48");
    ui_view_mount(container, toolbar);

    for (int i = 0; i < 5; i++)
    {
        UiView *item = ui_panel_create(GFX_DIM_GRAY);
        ui_view_layout(item, "m-s-4 dock-start min-w-64");
        ui_view_mount(toolbar, item);
    }

    ui_win_mount(self, container);

    UiView *button = ui_button_create_with_text(str$("HELLO WORLD!"));
    ui_view_layout(button, "m-s-4 dock-start");
    ui_view_mount(toolbar, button);

    UiView *button1 = ui_button_create_with_text(str$("HELLO FRIENDS!"));
    ui_view_layout(button1, "m-s-4 dock-start");
    ui_view_mount(toolbar, button1);

    UiView *icon1 = ui_icon_create();
    ui_view_layout(icon1, "dock-fill");
    ui_view_mount(container, icon1);

    return self;
}

int main(int, char const *[])
{
    UiApp app;
    ui_app_init(&app);

    UiWin *win = demo_win_create(&app);
    ui_win_show(win);

    int result = ui_app_run(&app);

    ui_win_deref(win);
    ui_app_deinit(&app);

    return result;
}
