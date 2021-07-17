#pragma once

#define base_cast(SELF) \
    (&(SELF)->base)

#define impl_cast(SELF) \
    (&(SELF)->_impl)

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
