#include <brutal/io.h>
#include <brutal/ui.h>

UiView *about_informations(void)
{
    UiView *label_title = ui_text_create(str$("BRUTAL"), UI_FONT_TITLE);

    UiView *label_version = ui_text_create(str$("Milestone 4 @ 3b96f95"), UI_FONT_BODY);

    UiView *label_website = ui_text_create(str$("brutal.smnx.sh"), UI_FONT_LINK);

    UiView *label_license = ui_text_create(str$("The brutal operating system and its core components are licensed under the MIT License."), UI_FONT_BODY);

    UiView *button = ui_button_create_with_text(str$("OK"));
    button->layout.placement = M_GRAVITY_END | M_GRAVITY_VFILL;

    UiView *container = ui_panel_create(UI_COLOR_BASE00);
    ui_view_layout(container, (UiLayout){
                                  .type = UI_LAYOUT_FLEX,
                                  .gaps = m_vec2(12, 0),
                                  .grow = 1,
                                  .flow = M_FLOW_TOP_TO_BOTTOM,
                                  .margin = m_spacing_all(16),
                              });

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
    UiWin *self = ui_win_create(app, m_rect(150, 150, 500, 300), UI_WIN_NORMAL);

    UiView *container = ui_panel_create(UI_COLOR_BASE00);
    ui_view_layout(container, (UiLayout){.type = UI_LAYOUT_FLEX});
    ui_win_mount(self, container);

    UiView *logo = ui_panel_create(UI_COLOR_BASE09);
    ui_view_layout(logo, (UiLayout){.grow = 1});

    ui_view_mount(container, logo);
    ui_view_mount(container, about_informations());

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
