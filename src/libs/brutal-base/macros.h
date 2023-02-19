#pragma once

// Align the nearest _lower_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 8
// ex: 7 with align = 8 -> 0
#define align_down$(ADDR, ALIGN) ((ADDR) & ~((ALIGN)-1))

// Align the nearest _upper_ aligned address
// ex: 8 with align = 8 -> 8
// ex: 9 with align = 8 -> 16
// ex: 7 with align = 8 -> 8
#define align_up$(ADDR, ALIGN) (((ADDR) + (ALIGN)-1) & ~((ALIGN)-1))

// Return the number of element in an array
#define array_len$(ARR) (sizeof(ARR) / sizeof(ARR[0]))

// Wait for a specific condition to become true
#define wait_for$(check) \
    while (!(check))     \
    {                    \
    };

// Swap two the content of two variables
#define swap$(X, Y)                     \
    do                                  \
    {                                   \
        typeof(*(X)) __swap_tmp = *(X); \
        *(X) = *(Y);                    \
        *(Y) = __swap_tmp;              \
    } while (0)

// clang-format off

#define _MatchSigned(EXPR) \
        char: EXPR,        \
        short: EXPR,       \
        int: EXPR,         \
        long: EXPR,        \
        long long: EXPR

#define _MatchUnsigned(EXPR)      \
        unsigned char: EXPR,      \
        unsigned short: EXPR,     \
        unsigned int: EXPR,       \
        unsigned long: EXPR,      \
        unsigned long long: EXPR

#define _MatchFloat(EXPR) \
        float: EXPR,      \
        double: EXPR

#define _MatchConst(T, EXPR) \
        T: EXPR,             \
        const T: EXPR

// clang-format on

#define __concat$(LHS, RHS) LHS##RHS

#define concat$(LHS, RHS) __concat$(LHS, RHS)

#define __stringify$(SYM) #SYM

#define stringify$(SYM) __stringify$(SYM)

#define var$(NAME) concat$(NAME, __LINE__)

#define defer$(BEGIN, END) for (int var$(__i) = (BEGIN, 0); !var$(__i); (var$(__i) += 1, END))

#define cond_defer$(BEGIN, END) for (int var$(__i) = BEGIN; var$(__i); (var$(__i) -= 1, END))
