#pragma once

#include <brutal-base>

#include "log.h"

#define assert_not_null(EXPR) (                      \
    {                                                \
        if (UNLIKELY(EXPR == nullptr))               \
        {                                            \
            panic$("{} is equal to nullptr", #EXPR); \
        }                                            \
    })

#define assert_truth(EXPR) (              \
    {                                     \
        typeof(EXPR) __value = (EXPR);    \
                                          \
        if (UNLIKELY(!__value))           \
        {                                 \
            panic$("{} is false", #EXPR); \
        }                                 \
    })

#define assert_br_success(EXPR) (                                                                                  \
    {                                                                                                              \
        BrResult __value = (EXPR);                                                                                 \
                                                                                                                   \
        if (UNLIKELY(__value != BR_SUCCESS))                                                                       \
        {                                                                                                          \
            panic$("{} is not equal to BR_SUCCESS (but is equal to: '{}') ", #EXPR, br_result_to_string(__value)); \
        }                                                                                                          \
    })

#define assert_falsity(EXPR) (           \
    {                                    \
        typeof(EXPR) __value = (EXPR);   \
                                         \
        if (UNLIKELY(__value))           \
        {                                \
            panic$("{} is true", #EXPR); \
        }                                \
    })

#define assert_equal(LHS, RHS) (                      \
    {                                                 \
        typeof(LHS) __lhs = LHS;                      \
        typeof(RHS) __rhs = RHS;                      \
                                                      \
        if (UNLIKELY(__lhs != __rhs))                 \
        {                                             \
            panic$("{} == {}: {} is not equal to {}", \
                   #LHS, #RHS, __lhs, __rhs);         \
        }                                             \
    })

#define assert_not_equal(LHS, RHS) (              \
    {                                             \
        typeof(LHS) __lhs = LHS;                  \
        typeof(RHS) __rhs = RHS;                  \
                                                  \
        if (UNLIKELY(__lhs == __rhs))             \
        {                                         \
            panic$("{} != {}: {} is equal to {}", \
                   #LHS, #RHS, __lhs, __rhs);     \
        }                                         \
    })

#define assert_str_equal(LHS, RHS) (                      \
    {                                                     \
        typeof(LHS) __lhs = LHS;                          \
        typeof(RHS) __rhs = RHS;                          \
                                                          \
        if (UNLIKELY(!str_eq(__lhs, __rhs)))              \
        {                                                 \
            panic$("{} == {}: '{}' is not equal to '{}'", \
                   #LHS, #RHS, __lhs, __rhs);             \
        }                                                 \
    })

#define assert_str_not_equal(LHS, RHS) (              \
    {                                                 \
        typeof(LHS) __lhs = LHS;                      \
        typeof(RHS) __rhs = RHS;                      \
                                                      \
        if (UNLIKELY(str_eq(__lhs, __rhs)))           \
        {                                             \
            panic$("{} != {}: '{}' is equal to '{}'", \
                   #LHS, #RHS, __lhs, __rhs);         \
        }                                             \
    })

#define assert_greater_than(LHS, RHS) (                  \
    {                                                    \
        typeof(LHS) __lhs = LHS;                         \
        typeof(RHS) __rhs = RHS;                         \
                                                         \
        if (UNLIKELY(!(__lhs > __rhs)))                  \
        {                                                \
            panic$("{} > {}: {} is not greater than {}", \
                   #LHS, #RHS, __lhs, __rhs);            \
        }                                                \
    })

#define assert_greater_equal(LHS, RHS) (                           \
    {                                                              \
        typeof(LHS) __lhs = LHS;                                   \
        typeof(RHS) __rhs = RHS;                                   \
                                                                   \
        if (!(__lhs >= __rhs))                                     \
        {                                                          \
            panic$("{} >= {}: {} is not greater or equal than {}", \
                   #LHS, #RHS, __lhs, __rhs);                      \
        }                                                          \
    })

#define assert_lower_than(LHS, RHS) (                  \
    {                                                  \
        typeof(LHS) __lhs = LHS;                       \
        typeof(RHS) __rhs = RHS;                       \
                                                       \
        if (UNLIKELY(!(__lhs < __rhs)))                \
        {                                              \
            panic$("{} < {}: {} is not lower than {}", \
                   #LHS, #RHS, __lhs, __rhs);          \
        }                                              \
    })

#define assert_lower_equal(LHS, RHS) (                           \
    {                                                            \
        typeof(LHS) __lhs = LHS;                                 \
        typeof(RHS) __rhs = RHS;                                 \
                                                                 \
        if (UNLIKELY(!(__lhs <= __rhs)))                         \
        {                                                        \
            panic$("{} <= {}: {} is not lower or equal than {}", \
                   #LHS, #RHS, __lhs, __rhs);                    \
        }                                                        \
    })
