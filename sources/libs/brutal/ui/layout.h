#pragma once

#include <brutal/gfx/color.h>
#include <brutal/gfx/font.h>
#include <brutal/math/dock.h>
#include <brutal/math/flow.h>
#include <brutal/math/gravity.h>
#include <brutal/math/rect.h>
#include <brutal/math/spacing.h>
#include <brutal/parse/scan.h>

struct _UiView;

/* --- Size ----------------------------------------------------------------- */

typedef struct
{
    MVec2 min;
    MVec2 max;
} UiSize;

static inline MVec2 ui_size_apply(UiSize size, MVec2 vec)
{
    if (size.min.width)
    {
        vec.width = m_max(vec.width, size.min.width);
    }

    if (size.min.height)
    {
        vec.height = m_max(vec.height, size.min.height);
    }

    if (size.max.width)
    {
        vec.width = m_min(vec.width, size.max.width);
    }

    if (size.max.height)
    {
        vec.height = m_min(vec.height, size.max.height);
    }

    return vec;
}

/* --- Layouts -------------------------------------------------------------- */

typedef enum
{
    UI_LAYOUT_DOCK,
    UI_LAYOUT_FLEX,
    UI_LAYOUT_GRID,
} UiLayoutType;

typedef struct
{
    UiLayoutType type;

    UiSize size;
    MDock dock;
    MGravity gravity;
    MGravity placement;
    MSpacing padding;
    MSpacing margin;
    MFlow flow;
    float grow;
    MVec2 gaps;
} UiLayout;

MVec2 ui_layout_size(UiLayout const *self, struct _UiView *views[], size_t len);

void ui_layout_run(UiLayout const *layout, MRect container, struct _UiView *views[], size_t len);

void ui_layout_parse(UiLayout *layout, Scan *scan);

void ui_layout_parse_str(UiLayout *layout, Str str);
