#pragma once

#include <brutal/math.h>

typedef struct
{
    float line_ascend;
    float ascend;
    float captop;
    float descend;
    float line_descend;

    float advance;
} BFontMetrics;

typedef struct
{
    MRect capbound;
    MRect linebound;
    MVec2 baseline;
} BFontMesure;
