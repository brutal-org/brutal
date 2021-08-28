#pragma once

#include <brutal/io/print.h>
#include <brutal/log/locked.h>

#define assert_not_null(EXPR) (                      \
    {                                                \
        if (EXPR == nullptr)                         \
        {                                            \
            panic$("{} is equal to nullptr", #EXPR); \
        }                                            \
    })

#define assert_truth(EXPR) (              \
    {                                     \
        typeof(EXPR) __value = (EXPR);    \
                                          \
        if (!__value)                     \
        {                                 \
            panic$("{} is false", #EXPR); \
        }                                 \
    })

#define assert_falsity(EXPR) (           \
    {                                    \
        typeof(EXPR) __value = (EXPR);   \
                                         \
        if (__value)                     \
        {                                \
            panic$("{} is true", #EXPR); \
        }                                \
    })

#define assert_equal(LHS, RHS) (                      \
    {                                                 \
        typeof(LHS) __lhs = LHS;                      \
        typeof(RHS) __rhs = RHS;                      \
                                                      \
        if (__lhs != __rhs)                           \
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
        if (__lhs == __rhs)                       \
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
        if (!str_eq(__lhs, __rhs))                        \
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
        if (str_eq(__lhs, __rhs))                     \
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
        if (!(__lhs > __rhs))                            \
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
        if (!(__lhs < __rhs))                          \
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
        if (!(__lhs <= __rhs))                                   \
        {                                                        \
            panic$("{} <= {}: {} is not lower or equal than {}", \
                   #LHS, #RHS, __lhs, __rhs);                    \
        }                                                        \
    })

#define assert_unreachable() ({ panic$("We reach the unreachable"); })
