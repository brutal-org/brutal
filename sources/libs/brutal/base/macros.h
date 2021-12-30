#pragma once

#define __CONCAT_IMPL(LHS, RHS) LHS##RHS
#define CONCAT(LHS, RHS) __CONCAT_IMPL(LHS, RHS)

// Align the nearest _lower_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 8
// ex: 7 with align = 8 -> 0
#define ALIGN_DOWN(ADDR, ALIGN) ((ADDR) & ~((ALIGN)-1))

// Align the nearest _upper_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 16
// ex: 7 with align = 8 -> 8
#define ALIGN_UP(ADDR, ALIGN) (((ADDR) + (ALIGN)-1) & ~((ALIGN)-1))

// Return the number of element in an array
#define ARRAY_LEN(ARR) (sizeof(ARR) / sizeof(ARR[0]))

// Return the filename of the current translation unite
#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

// Wait for a specific condition to become true
#define WAIT_FOR(check) \
    while (!(check))    \
    {                   \
    };

// Get the size if a compound type member
#define MBR_SIZE(T, MEMBER) sizeof(((T *)0)->MEMBER)

// Get the type of a compound type member
#define MBR_TYPE(T, MEMBER) typeof(((T *)0)->MEMBER)

// Swap two the content of two variables
#define SWAP(X, Y)            \
    do                        \
    {                         \
        typeof(X) SWAP = (X); \
        (X) = (Y);            \
        (Y) = SWAP;           \
    } while (0)

// Mark a symbole as deprecated
//
//  Example:
//  DEPRECATED(old_and_unsafe_symbole)
//
#define DEPRECATED(SYM) DEPRECATED_##SYM
