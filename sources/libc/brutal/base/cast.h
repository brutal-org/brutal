#pragma once

#define base_cast(instance) \
    (&(instance)->base)

#define union_cast(T, x) ( \
    {                      \
        union              \
        {                  \
            typeof(x) a;   \
            T b;           \
        } u;               \
        u.a = x;           \
        u.b;               \
    })
