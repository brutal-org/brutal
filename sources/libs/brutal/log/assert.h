#pragma once

#include <brutal/io/print.h>
#include <brutal/log/locked.h>

#define assert_not_null(expr) (                     \
    {                                               \
        if (expr == nullptr)                        \
        {                                           \
            panic("{} is equal to nullptr", #expr); \
        }                                           \
    })

#define assert_truth(expr) (             \
    {                                    \
        auto __value = (expr);           \
                                         \
        if (!__value)                    \
        {                                \
            panic("{} is false", #expr); \
        }                                \
    })

#define assert_falsity(expr) (          \
    {                                   \
        auto __value = (expr);          \
                                        \
        if (__value)                    \
        {                               \
            panic("{} is true", #expr); \
        }                               \
    })

#define assert_equal(lhs_expr, rhs_expr) (                         \
    {                                                              \
        auto __lhs_value = lhs_expr;                               \
        auto __rhs_value = rhs_expr;                               \
                                                                   \
        if (__lhs_value != __rhs_value)                            \
        {                                                          \
            panic("{} == {}: {} is not equal to {}",               \
                  #lhs_expr, #rhs_expr, __lhs_value, __rhs_value); \
        }                                                          \
    })

#define assert_not_equal(lhs_expr, rhs_expr) (                     \
    {                                                              \
        auto __lhs_value = lhs_expr;                               \
        auto __rhs_value = rhs_expr;                               \
                                                                   \
        if (__lhs_value == __rhs_value)                            \
        {                                                          \
            panic("{} != {}: {} is equal to {}",                   \
                  #lhs_expr, #rhs_expr, __lhs_value, __rhs_value); \
        }                                                          \
    })

#define assert_str_equal(lhs_expr, rhs_expr) (                     \
    {                                                              \
        auto __lhs_value = lhs_expr;                               \
        auto __rhs_value = rhs_expr;                               \
                                                                   \
        if (!str_eq(__lhs_value, __rhs_value))                     \
        {                                                          \
            panic("{} == {}: '{}' is not equal to '{}'",           \
                  #lhs_expr, #rhs_expr, __lhs_value, __rhs_value); \
        }                                                          \
    })

#define assert_str_not_equal(lhs_expr, rhs_expr) (                 \
    {                                                              \
        auto __lhs_value = lhs_expr;                               \
        auto __rhs_value = rhs_expr;                               \
                                                                   \
        if (str_eq(__lhs_value, __rhs_value))                      \
        {                                                          \
            panic("{} != {}: '{}' is equal to '{}'",               \
                  #lhs_expr, #rhs_expr, __lhs_value, __rhs_value); \
        }                                                          \
    })

#define assert_greater_than(lhs_expr, rhs_expr) (                  \
    {                                                              \
        auto __lhs_value = lhs_expr;                               \
        auto __rhs_value = rhs_expr;                               \
                                                                   \
        if (!(__lhs_value > __rhs_value))                          \
        {                                                          \
            panic("{} > {}: {} is not greater than {}",            \
                  #lhs_expr, #rhs_expr, __lhs_value, __rhs_value); \
        }                                                          \
    })

#define assert_greater_equal(lhs_expr, rhs_expr) (                 \
    {                                                              \
        auto __lhs_value = lhs_expr;                               \
        auto __rhs_value = rhs_expr;                               \
                                                                   \
        if (!(__lhs_value >= __rhs_value))                         \
        {                                                          \
            panic("{} >= {}: {} is not greater or equal than {}",  \
                  #lhs_expr, #rhs_expr, __lhs_value, __rhs_value); \
        }                                                          \
    })

#define assert_lower_than(lhs_expr, rhs_expr) (                    \
    {                                                              \
        auto __lhs_value = lhs_expr;                               \
        auto __rhs_value = rhs_expr;                               \
                                                                   \
        if (!(__lhs_value < __rhs_value))                          \
        {                                                          \
            panic("{} < {}: {} is not lower than {}",              \
                  #lhs_expr, #rhs_expr, __lhs_value, __rhs_value); \
        }                                                          \
    })

#define assert_lower_equal(lhs_expr, rhs_expr) (                   \
    {                                                              \
        auto __lhs_value = lhs_expr;                               \
        auto __rhs_value = rhs_expr;                               \
                                                                   \
        if (!(__lhs_value <= __rhs_value))                         \
        {                                                          \
            panic("{} <= {}: {} is not lower or equal than {}",    \
                  #lhs_expr, #rhs_expr, __lhs_value, __rhs_value); \
        }                                                          \
    })

#define assert_unreachable() ({ panic("We reach the unreachable"); })
