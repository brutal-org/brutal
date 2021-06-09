#pragma once

#define nullptr ((void *)0)

#define auto __auto_type

#define static_assert _Static_assert

#define PACKED __attribute__((packed))

#define ALIGNED(__align) __attribute__((aligned(__align)))

#define UNUSED(expr) ((void)expr)

#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define CLEANUP(func) __attribute__((__cleanup__(func)))

#define CONCAT_(lhs, rhs) lhs##rhs
#define CONCAT(lhs, rhs) CONCAT_(lhs, rhs)

#define STMT(expr) \
    do             \
    {              \
        expr       \
    } while (0)

// Align the nearest _lower_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 16
// ex: 7 with align = 8 -> 0
#define ALIGN_DOWN(__addr, __align) ((__addr) & ~((__align)-1))

// Align the nearest _upper_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 16
// ex: 7 with align = 8 -> 8
#define ALIGN_UP(__addr, __align) (((__addr) + (__align)-1) & ~((__align)-1))

#define ARRAY_LENGTH(__array) (sizeof(__array) / sizeof(__array[0]))

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
