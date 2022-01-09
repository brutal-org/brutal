#include <brutal/alloc.h>
#include <brutal/io.h>
#include <brutal/ui.h>

UiView *about_informations(void)
{
    UiView *label_title = ui_text_create(str$("BRUTAL"), GFX_UI_BASE09);
    ui_view_style(label_title, (UiStyle){.dock = UI_DOCK_TOP});

    UiView *label_version = ui_text_create(str$("Milestone 4 @ 3b96f95"), GFX_UI_BASE09);
    ui_view_style(label_version, (UiStyle){.dock = UI_DOCK_TOP});

    UiView *label_website = ui_text_create(str$("brutal.smnx.sh"), GFX_UI_ACCENT);
    ui_view_style(label_website, (UiStyle){.dock = UI_DOCK_TOP});

    UiView *label_license = ui_text_create(str$("The brutal operating system and its core components are licensed under the MIT License."), GFX_UI_BASE09);
    ui_view_style(label_license, (UiStyle){.dock = UI_DOCK_TOP});

    UiView *button = ui_button_create_with_text(str$("OK"));
    ui_view_style(button, (UiStyle){.dock = UI_DOCK_BOTTOM, .size.max = m_vec2(160, 36)});

    UiView *container = ui_panel_create(GFX_UI_BASE00);
    ui_view_style(container, (UiStyle){.dock = UI_DOCK_FILL, .margin = ui_spacing_all(16)});

    ui_view_mount(container, label_title);
    ui_view_mount(container, label_version);
    ui_view_mount(container, label_website);
    ui_view_mount(container, label_license);
    ui_view_mount(container, button);

    return container;
}

UiWin *about_win_create(UiApp *app)
{
    UiWin *self = ui_win_create(app, m_rect(0, 0, 500, 300), 0);

    UiView *container = ui_panel_create(GFX_UI_BASE00);
    ui_view_style(
        container,
        (UiStyle){
            .layout = UI_LAYOUT_DOCK,
            .flow = M_FLOW_LEFT_TO_RIGHT,
        });
    ui_win_mount(self, container);

    UiView *logo = ui_panel_create(GFX_UI_BASE09);
    ui_view_style(logo, (UiStyle){.dock = UI_DOCK_START, .size.max.width = 234});

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
