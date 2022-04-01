#pragma once

#include <math.h>

typedef struct
{
    float v;
} MDegree;

typedef struct
{
    float v;
} MRadian;

static inline MDegree m_deg(float value)
{
    return (MDegree){value};
}

static inline MRadian m_rad(float value)
{
    return (MRadian){value};
}

static inline MRadian m_deg2rad(MDegree deg)
{
    return (MRadian){(deg.v) * (M_PI / 180.f)};
}

static inline MDegree m_rad2deg(MRadian rad)
{
    return (MDegree){(rad.v) * (180.f / M_PI)};
}
