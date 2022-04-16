#include <brutal-io>
#include <brutal-ui>

UiView *about_informations(void)
{
    UiView *label_title = ui_text_create(str$("BRUTAL"), UI_FONT_TITLE);
    UiView *label_version = ui_text_create(str$("Milestone 5 @ 3b96f95"), UI_FONT_BODY);
    UiView *label_website = ui_text_create(str$("brutal.smnx.sh"), UI_FONT_LINK);
    UiView *label_license = ui_text_create(str$("The brutal operating system and its core components are licensed under the MIT License."), UI_FONT_BODY);
    UiView *button = ui_button_create_with_text(str$("OK"));
    ui_view_layout(button, "g-end g-vfill");

    UiView *container = ui_panel_create(UI_COLOR_BASE00);
    ui_view_layout(container, "flex gaps-12 grow-1 ttb m-16");

    ui_view_mount(container, label_title);
    ui_view_mount(container, label_version);
    ui_view_mount(container, label_website);
    ui_view_mount(container, label_license);
    ui_view_mount(container, ui_spacer_create());
    ui_view_mount(container, button);

    return container;
}

UiWin *about_win_create(UiApp *app)
{
    UiWin *self = ui_win_create(app, m_rectf(150, 150, 500, 300), UI_WIN_NORMAL);

    UiView *container = ui_panel_create(UI_COLOR_BASE00);
    ui_view_layout(container, "flex");
    ui_win_mount(self, container);

    UiView *logo = ui_panel_create(UI_COLOR_BASE09);
    ui_view_layout(logo, "grow-1");
    ui_view_mount(container, logo);

    UiView *infos = about_informations();
    ui_view_mount(container, infos);

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
