#pragma once

#include <brutal/math/flow.h>

typedef struct
{
    float start;
    float top;
    float end;
    float bottom;
} MSpacing;

MSpacing m_spacing_all(float value);

MRect m_spacing_shrink(MSpacing spacing, MFlow flow, MRect rect);

MRect m_spacing_grow(MSpacing spacing, MFlow flow, MRect rect);
