#include <brutal/alloc/global.h>
#include <brutal/ui/button.h>
#include <brutal/ui/text.h>

void ui_button_paint(UiView *self, Gfx *gfx)
{
    if (ui_button$(self)->press)
    {
        gfx_fill(gfx, gfx_paint_fill(GFX_BLUE));
    }
    else if (ui_button$(self)->over)
    {
        gfx_fill(gfx, gfx_paint_fill(GFX_GREEN));
    }
    else if (self->flags & UI_VIEW_ENABLED)
    {
        gfx_fill(gfx, gfx_paint_fill(GFX_MAGENTA));
    }
    else
    {
        gfx_fill(gfx, gfx_paint_fill(GFX_RED));
    }

    gfx_rect(gfx, ui_view_container(self));

    gfx_fill(gfx, gfx_paint_fill(GFX_BLACK));
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

    self->flags = UI_VIEW_GREEDY;
    self->paint = ui_button_paint;
    self->event = ui_button_event;

    return self;
}

UiView *ui_button_create_with_text(Str text)
{
    UiView *self = ui_button_create();
    UiView *label = ui_text_create(text);
    ui_view_style(label, (UiStyle){.dock = UI_DOCK_FILL});
    ui_view_mount(self, label);

    return self;
}
