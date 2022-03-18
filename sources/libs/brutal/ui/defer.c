#include <brutal/ui/defer.h>

void ui_defer_init(UiDefer *self, UiDeferFn *fn, void *context)
{
    self->fn = fn;
    self->context = context;
    self->deadline = -1;

    ui_app_attach_defer(ui_app_self(), self);
}

void ui_defer_deinit(UiDefer *self)
{
    ui_app_detach_defer(ui_app_self(), self);
}

void ui_defer_sched(UiDefer *self)
{
    self->deadline = 0;
}

void ui_defer_delay(UiDefer *self, Tick tick)
{
    self->deadline = time_now_ms() + tick;
}

void ui_defer_cancel(UiDefer *self)
{
    self->deadline = -1;
}

void ui_defer_invoke(UiDefer *self)
{
    self->fn(self->context, self);
    self->deadline = -1;
}
