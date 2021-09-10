#pragma once

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define CONCAT_(lhs, rhs) lhs##rhs
#define CONCAT(lhs, rhs) CONCAT_(lhs, rhs)

#define STMT(expr) \
    do             \
    {              \
        expr       \
    } while (0)

// Align the nearest _lower_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 8
// ex: 7 with align = 8 -> 0
#define ALIGN_DOWN(__addr, __align) ((__addr) & ~((__align)-1))

// Align the nearest _upper_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 16
// ex: 7 with align = 8 -> 8
#define ALIGN_UP(__addr, __align) (((__addr) + (__align)-1) & ~((__align)-1))

#define ARRAY_LENGTH(__array) (sizeof(__array) / sizeof(__array[0]))

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define WAIT_FOR(check) \
    while (!(check))    \
    {                   \
    };

#define member_size(T, MEMBER) sizeof(((T *)0)->MEMBER)

#define member_type(T, MEMBER) typeof(((T *)0)->MEMBER)

#define swap(X, Y)            \
    do                        \
    {                         \
        typeof(X) SWAP = (X); \
        (X) = (Y);            \
        (Y) = SWAP;           \
    } while (0)
