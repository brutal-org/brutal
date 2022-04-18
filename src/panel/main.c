#include <brutal-io>
#include <brutal-ui>

UiWin *about_win_create(UiApp *app)
{
    UiWin *self = ui_win_create(app, m_rectf(0, 0, 800, 48), UI_WIN_DOCK_TOP);

    UiView *container = ui_panel_create(UI_COLOR_BASE00);
    ui_view_layout(container, "flex p-8");

    UiView *btn_applications = ui_button_create_with_text(str$("Applications"));
    ui_view_mount(container, btn_applications);

    UiView *lbl_time = ui_text_create(str$("Thue 11 Jan 16:26"), UI_FONT_BODY);
    ui_view_layout(lbl_time, "grow-1 place-center");

    ui_view_mount(container, lbl_time);

    UiView *btn_options = ui_button_create_with_text(str$("..."));
    ui_view_mount(container, btn_options);

    ui_win_mount(self, container);

    return self;
}

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;

    UiApp app;
    ui_app_init(&app);

    UiWin *win = about_win_create(&app);
    ui_win_show(win);

    int result = ui_app_run(&app);

    ui_win_deref(win);
    ui_app_deinit(&app);

    return result;
}
