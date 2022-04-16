#pragma once

#include "flow.h"

typedef struct
{
    float start;
    float top;
    float end;
    float bottom;
} MSpacing;

MSpacing m_spacing_all(float value);

MRectf m_spacing_shrink(MSpacing spacing, MFlow flow, MRectf rect);

MRectf m_spacing_grow(MSpacing spacing, MFlow flow, MRectf rect);
