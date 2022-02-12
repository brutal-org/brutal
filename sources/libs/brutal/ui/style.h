#pragma once

#include <brutal/gfx/color.h>
#include <brutal/gfx/font.h>
#include <brutal/math/dock.h>
#include <brutal/math/flow.h>
#include <brutal/math/gravity.h>
#include <brutal/math/rect.h>
#include <brutal/math/spacing.h>

/* --- Size ----------------------------------------------------------------- */

typedef struct
{
    MVec2 min;
    MVec2 max;
    bool square;
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

    if (size.square)
    {
        float v = m_min(vec.width, vec.height);
        vec.width = v;
        vec.height = v;
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
