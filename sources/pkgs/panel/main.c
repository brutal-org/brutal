#include <brutal/io.h>
#include <brutal/ui.h>

UiWin *about_win_create(UiApp *app)
{
    UiWin *self = ui_win_create(app, m_rect(0, 0, 1280, 48), 0);

    UiView *container = ui_panel_create(GFX_UI_BASE00);
    ui_view_style(
        container,
        (UiStyle){
            .layout = UI_LAYOUT_FLEX,
            .padding = {
                .start = 6,
                .end = 6,
            },
        });

    UiView *btn_applications = ui_button_create_with_text(str$("Applications"));
    ui_view_style(btn_applications, (UiStyle){.size.max = m_vec2(160, 36), .placement = M_GRAVITY_CENTER});
    ui_view_mount(container, btn_applications);

    UiView *lbl_time = ui_text_create(str$("Thue 11 Jan 16:26"), gfx_font_builtin(), GFX_UI_BASE09);
    ui_view_style(lbl_time, (UiStyle){.grow = 1, .placement = M_GRAVITY_CENTER});
    ui_view_mount(container, lbl_time);

    UiView *btn_options = ui_button_create_with_text(str$("..."));
    ui_view_style(btn_options, (UiStyle){.size = {.max.y = 36, .square = true}, .placement = M_GRAVITY_CENTER});
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
