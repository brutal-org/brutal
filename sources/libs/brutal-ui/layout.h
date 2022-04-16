#pragma once

#include <brutal-gfx>
#include <brutal-math>

struct _UiView;

/* --- Size ----------------------------------------------------------------- */

typedef struct
{
    MVec2f min;
    MVec2f max;
} UiSize;

static inline MVec2f ui_size_apply(UiSize size, MVec2f vec)
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
    MVec2f gaps;
} UiLayout;

MVec2f ui_layout_size(UiLayout const *self, struct _UiView *views[], size_t len);

void ui_layout_run(UiLayout const *layout, MRectf container, struct _UiView *views[], size_t len);
