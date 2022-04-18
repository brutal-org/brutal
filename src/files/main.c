#include <brutal-io>
#include <brutal-ui>

UiWin *files_win_create(UiApp *app)
{
    UiWin *self = ui_win_create(app, m_rectf(0, 0, 700, 500), UI_WIN_NORMAL);

    UiView *container = ui_panel_create(UI_COLOR_BASE00);
    ui_view_layout(container, "flex");
    ui_win_mount(self, container);

    return self;
}

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;

    UiApp app = {};
    ui_app_init(&app);

    UiWin *win = files_win_create(&app);
    ui_win_show(win);

    int result = ui_app_run(&app);

    ui_win_deref(win);
    ui_app_deinit(&app);

    return result;
}
