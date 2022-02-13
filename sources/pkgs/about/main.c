#include <brutal/alloc.h>
#include <brutal/io.h>
#include <brutal/ui.h>

UiView *about_informations(void)
{
    UiView *label_title = ui_text_create(str$("BRUTAL"), UI_FONT_TITLE);
    ui_view_layout(label_title, (UiLayout){.dock = M_DOCK_TOP});

    UiView *label_version = ui_text_create(str$("Milestone 4 @ 3b96f95"), UI_FONT_BODY);
    ui_view_layout(label_version, (UiLayout){.dock = M_DOCK_TOP});

    UiView *label_website = ui_text_create(str$("brutal.smnx.sh"), UI_FONT_LINK);
    ui_view_layout(label_website, (UiLayout){.dock = M_DOCK_TOP});

    UiView *label_license = ui_text_create(str$("The brutal operating system and its core components are licensed under the MIT License."), UI_FONT_BODY);
    ui_view_layout(label_license, (UiLayout){.dock = M_DOCK_TOP});

    UiView *button = ui_button_create_with_text(str$("OK"));
    ui_view_layout(button, (UiLayout){.dock = M_DOCK_BOTTOM, .size.max = m_vec2(160, 36)});

    UiView *container = ui_panel_create(UI_COLOR_BASE00);
    ui_view_layout(container, (UiLayout){.dock = M_DOCK_FILL, .margin = m_spacing_all(16)});

    ui_view_mount(container, label_title);
    ui_view_mount(container, label_version);
    ui_view_mount(container, label_website);
    ui_view_mount(container, label_license);
    ui_view_mount(container, button);

    return container;
}

UiWin *about_win_create(UiApp *app)
{
    UiWin *self = ui_win_create(app, m_rect(150, 150, 500, 300), UI_WIN_NORMAL);

    UiView *container = ui_panel_create(UI_COLOR_BASE00);
    ui_view_layout(container, (UiLayout){.type = UI_LAYOUT_DOCK, .flow = M_FLOW_LEFT_TO_RIGHT});
    ui_win_mount(self, container);

    UiView *logo = ui_panel_create(UI_COLOR_BASE09);
    ui_view_layout(logo, (UiLayout){.dock = M_DOCK_START, .size.max.width = 234});

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

    IoFile file;
    assert_truth(io_file_open(&file, str$("sysroot/pkgs/font-inter/Inter.sfnc.gz")).succ);
    SSFN2Collection collection;
    ssfn2_load(io_file_rseek(&file), &collection, alloc_global());
    ui_app_font(&app, ssfn2_font(&collection));

    UiWin *win = about_win_create(&app);
    ui_win_show(win);

    int result = ui_app_run(&app);

    ui_win_deref(win);
    ui_app_deinit(&app);

    return result;
}
