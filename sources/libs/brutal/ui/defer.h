#pragma once

#include <brutal/time.h>
#include <brutal/ui/app.h>

typedef struct _UiDefer UiDefer;

typedef void UiDeferFn(void *context, struct _UiDefer *defer);

struct _UiDefer
{
    bool removed;
    Tick deadline;
    UiDeferFn *fn;
    void *context;
};

void ui_defer_init(UiDefer *self, UiDeferFn *fn, void *context);

void ui_defer_deinit(UiDefer *self);

void ui_defer_sched(UiDefer *self);

void ui_defer_delay(UiDefer *self, Tick tick);

void ui_defer_cancel(UiDefer *self);

void ui_defer_invoke(UiDefer *self);
