#pragma once

#define range_t(T) \
    struct         \
    {              \
        T base;    \
        T size;    \
    }

#define range_cast(T, range)   \
    (T)                        \
    {                          \
        range.base, range.size \
    }

#define range_empty(range) \
    ((range).size == 0)

#define range_any(range) \
    ((range).size > 0)

#define range_end(range) \
    ((range).base + (range).size - 1)

#define range_contain(range, addr) \
    (((range).base <= (addr)) && ((range).size + (range).base >= (addr)))
