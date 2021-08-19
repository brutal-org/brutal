#pragma once

#define base$(SELF) \
    (&(SELF)->base)

#define impl$(SELF) \
    (&(SELF)->_impl)

#define union$(T, x) (   \
    {                    \
        union            \
        {                \
            typeof(x) a; \
            T b;         \
        } u;             \
        u.a = x;         \
        u.b;             \
    })
