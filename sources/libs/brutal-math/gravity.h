#pragma once

#include "flow.h"
#include "rect.h"

#define M_GRAVITY_NONE (0)

#define M_GRAVITY_START (1 << 0)
#define M_GRAVITY_TOP (1 << 1)

#define M_GRAVITY_END (1 << 2)
#define M_GRAVITY_BOTTOM (1 << 3)

#define M_GRAVITY_VSTRETCH (1 << 4)
#define M_GRAVITY_HSTRETCH (1 << 5)

#define M_GRAVITY_VCENTER (1 << 6)
#define M_GRAVITY_HCENTER (1 << 7)

#define M_GRAVITY_VFILL (M_GRAVITY_VSTRETCH | M_GRAVITY_TOP)
#define M_GRAVITY_HFILL (M_GRAVITY_HSTRETCH | M_GRAVITY_START)

#define M_GRAVITY_COVER (1 << 8)
#define M_GRAVITY_FIT (1 << 9)
#define M_GRAVITY_CENTER (M_GRAVITY_HCENTER | M_GRAVITY_VCENTER)
#define M_GRAVITY_FILL (M_GRAVITY_VFILL | M_GRAVITY_HFILL)

typedef uint64_t MGravity;

MRectf m_gravity_apply(MGravity gravity, MFlow flow, MRectf content, MRectf container);
