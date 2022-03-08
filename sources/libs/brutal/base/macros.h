#pragma once

#define __concat$(LHS, RHS) LHS##RHS

#define concat$(LHS, RHS) __concat$(LHS, RHS)

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

#define var$(NAME) concat$(NAME, __LINE__)
#define defer$(BEGIN, END) for (int var$(__i) = (BEGIN, 0); !var$(__i); (var$(__i) += 1, END))
