#include <brutal/alloc/global.h>
#include <brutal/ui/button.h>
#include <brutal/ui/color.h>
#include <brutal/ui/font.h>
#include <brutal/ui/text.h>

void ui_button_paint(UiView *self, Gfx *gfx)
{
    if (self->flags & UI_VIEW_ENABLED)
    {
        gfx_fill_style(gfx, gfx_paint_fill(UI_COLOR_BLUE));
    }
    else
    {
        gfx_fill_style(gfx, gfx_paint_fill(UI_COLOR_BASE04));
    }

    gfx_fill_rect(gfx, ui_view_container(self), 4);

    if (ui_button$(self)->press)
    {
        gfx_fill_style(gfx, gfx_paint_fill(gfx_color_with_alpha(UI_COLOR_BASE09, 75)));
        gfx_fill_rect(gfx, ui_view_container(self), 4);
    }
    else if (ui_button$(self)->over)
    {
        gfx_fill_style(gfx, gfx_paint_fill(gfx_color_with_alpha(UI_COLOR_BASE09, 50)));
        gfx_fill_rect(gfx, ui_view_container(self), 4);
    }
}

void ui_button_event(UiView *self, UiEvent *event)
{
    switch (event->type)
    {
    case UI_EVENT_MOUSE_UP:
        ui_button$(self)->press = false;
        ui_view_should_repaint(self);
        event->captured = true;
        break;

    case UI_EVENT_MOUSE_DOWN:
        ui_button$(self)->press = true;
        ui_view_should_repaint(self);
        event->captured = true;
        break;

    case UI_EVENT_ENTER:
        ui_button$(self)->over = true;
        ui_view_should_repaint(self);
        event->captured = true;
        break;

    case UI_EVENT_LEAVE:
        ui_button$(self)->over = false;
        ui_view_should_repaint(self);
        event->captured = true;
        break;

    default:
        break;
    }
}

UiView *ui_button_create(void)
{
    UiView *self = ui_view_create(UiButton);

    self->flags |= UI_VIEW_GREEDY;
    self->repaint = ui_button_paint;
    self->event = ui_button_event;

    ui_view_layout(self, (UiLayout){.size.max.height = 36});

    return self;
}

UiView *ui_button_create_with_text(Str text)
{
    UiView *self = ui_button_create();
    UiView *label = ui_text_create(text, UI_FONT_BODY);
    ui_view_layout(label, (UiLayout){.dock = M_DOCK_FILL, .gravity = M_GRAVITY_CENTER});
    ui_view_mount(self, label);

    return self;
}
