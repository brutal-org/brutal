#pragma once

#define Range(T) \
    struct       \
    {            \
        T base;  \
        T size;  \
    }

#define range_cast(T, RANGE) (        \
    {                                 \
        typeof(RANGE) _range = RANGE; \
                                      \
        (T){                          \
            _range.base,              \
            _range.size,              \
        };                            \
    })

#define range_empty(RANGE) \
    ((RANGE).size == 0)

#define range_any(RANGE) \
    ((RANGE).size > 0)

#define range_begin(RANGE) \
    ((RANGE).base)

#define range_end(RANGE) \
    ((RANGE).base + (RANGE).size)

#define range_contain(RANGE, ADDR) \
    (((RANGE).base <= (ADDR)) && ((RANGE).size + (RANGE).base >= (ADDR)))

#define range_contiguous(LHS, RHS) \
    ((range_begin(LHS) == range_end(RHS)) || (range_end(LHS) == range_begin(RHS)))

#define range_from_start_and_end(T, START, END) \
    (T) { STAR, END - START }

#define range_merge(LHS, RHS) (                                 \
    {                                                           \
        auto __start = MIN(range_begin(LHS), range_begin(RHS)); \
        auto __end = MAX(range_end(LHS), range_end(RHS)));      \
        range_from_start_and_end(typeof(LHS), __start, __end);  \
    })
