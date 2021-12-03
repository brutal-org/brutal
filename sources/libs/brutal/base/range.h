#pragma once

#include <brutal/base/macros.h>
#include <brutal/base/std.h>

#define Range(T) \
    struct       \
    {            \
        T base;  \
        T size;  \
    }

#define range$(T, RANGE) (            \
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

#define range_colide(LHS, RHS) (range_begin(LHS) < range_end(RHS) && range_end(LHS) > range_begin(RHS))

#define range_from_start_and_end(T, START, END) \
    (T) { (START), (END - START) }

#define range_merge(LHS, RHS) (                                     \
    {                                                               \
        AutoType __start = MIN(range_begin(LHS), range_begin(RHS)); \
        AutoType __end = MAX(range_end(LHS), range_end(RHS));       \
        range_from_start_and_end(typeof(LHS), __start, __end);      \
    })

#define range_half_under(SELF, SPLIT) (             \
    {                                               \
        typeof(SELF) __self = (SELF);               \
        typeof(SPLIT) __split = (SPLIT);            \
                                                    \
        typeof(SELF) result = (typeof(SELF)){};     \
                                                    \
        if (range_colide(__self, __split) &&        \
            __self.base < __split.base)             \
        {                                           \
            result = (typeof(SELF)){                \
                .base = __self.base,                \
                .size = __split.base - __self.base, \
            };                                      \
        }                                           \
                                                    \
        result;                                     \
    })

#define range_half_over(SELF, SPLIT) (                          \
    {                                                           \
        typeof(SELF) __self = (SELF);                           \
        typeof(SPLIT) __split = (SPLIT);                        \
                                                                \
        typeof(SELF) result = {};                               \
                                                                \
        if (range_colide(__self, __split) &&                    \
            range_end(__self) > range_end(__split))             \
        {                                                       \
            result = (typeof(SELF)){                            \
                .base = range_end(__split),                     \
                .size = range_end(__self) - range_end(__split), \
            };                                                  \
        }                                                       \
                                                                \
        result;                                                 \
    })

#define range_eq(LHS, RHS) ((LHS).base == (RHS).base && (LHS).size == (RHS).size)

#define range_mul(SELF, VAL) ((typeof(SELF)){.base = (SELF).base * (VAL), .size = (SELF).size * (VAL)})

#define range_div(SELF, VAL) ((typeof(SELF)){.base = (SELF).base / (VAL), .size = (SELF).size / (VAL)})

// alignment with higher size
// and lower address
#define range_align_higher(SELF, ALIGN) ((typeof(SELF)){.base = ALIGN_DOWN((SELF).base, (ALIGN)), .size = ALIGN_UP((SELF).size, (ALIGN))})

// alignment with lower size
// and higher address
#define range_align_lower(SELF, ALIGN) ((typeof(SELF)){.base = ALIGN_UP((SELF).base, (ALIGN)), .size = ALIGN_DOWN((SELF).size, (ALIGN))})

typedef Range(uint8_t) U8Range;
typedef Range(uint16_t) U16Range;
typedef Range(uint32_t) U32Range;
typedef Range(uint64_t) U64Range;
typedef Range(size_t) USizeRange;
