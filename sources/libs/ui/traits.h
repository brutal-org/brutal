#pragma once

#include <brutal/base.h>

/* --- Context -------------------------------------------------------------- */

#define UI_STATE (1 << 0)
#define UI_LAYOUT (1 << 1)
#define UI_VISUAL (1 << 2)

typedef bool UiBegin(void *ctx);
typedef void UiEnd(void *ctx);
typedef void *UiUse(void *ctx, int what);

typedef void *UiUpdate(void *ctx, int what);

typedef struct
{
    void *context;

    UiBegin *begin;
    UiEnd *end;
    UiUse *use;
    UiUpdate *update;
} Ui;
