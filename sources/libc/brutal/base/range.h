#pragma once

#define Range(T) \
    struct       \
    {            \
        T base;  \
        T size;  \
    }

#define range_cast(T, RANGE) (       \
    {                                \
        typeof(RANGE) range = RANGE; \
                                     \
        (T){                         \
            range.base,              \
            range.size,              \
        };                           \
    })

#define range_empty(RANGE) \
    ((RANGE).size == 0)

#define range_any(RANGE) \
    ((RANGE).size > 0)

#define range_end(RANGE) \
    ((RANGE).base + (RANGE).size)

#define range_contain(RANGE, ADDR) \
    (((RANGE).base <= (ADDR)) && ((RANGE).size + (RANGE).base >= (ADDR)))
