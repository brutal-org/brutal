#pragma once

#include <brutal/io/print.h>
#include <brutal/log/locked.h>

#define assert_not_null(expr) (                  \
    {                                            \
        if (expr == nullptr)                     \
        {                                        \
            panic(#expr " is equal to nullptr"); \
        }                                        \
    })

#define assert_truth(expr) (          \
    {                                 \
        if (!expr)                    \
        {                             \
            panic(#expr " is false"); \
        }                             \
    })

#define assert_falsity(expr) (       \
    {                                \
        if (expr)                    \
        {                            \
            panic(#expr " is true"); \
        }                            \
    })

#define assert_equal(lhs_expr, rhs_expr) (                                                     \
    {                                                                                          \
        auto lhs_value = lhs_expr;                                                             \
        auto rhs_value = rhs_expr;                                                             \
                                                                                               \
        if (lhs_value != rhs_value)                                                            \
        {                                                                                      \
            panic(#lhs_expr " == " #rhs_expr ": {} is not equal to {}", lhs_value, rhs_value); \
        }                                                                                      \
    })

#define assert_not_equal(lhs_expr, rhs_expr) (                                             \
    {                                                                                      \
        auto lhs_value = lhs_expr;                                                         \
        auto rhs_value = rhs_expr;                                                         \
                                                                                           \
        if (lhs_value == rhs_value)                                                        \
        {                                                                                  \
            panic(#lhs_expr " != " #rhs_expr ": {} is equal to {}", lhs_value, rhs_value); \
        }                                                                                  \
    })

#define assert_greater_than(lhs_expr, rhs_expr) (                                                 \
    {                                                                                             \
        auto lhs_value = lhs_expr;                                                                \
        auto rhs_value = rhs_expr;                                                                \
                                                                                                  \
        if (!(lhs_value > rhs_value))                                                             \
        {                                                                                         \
            panic(#lhs_expr " > " #rhs_expr ": {} is not greater than {}", lhs_value, rhs_value); \
        }                                                                                         \
    })

#define assert_greater_equal(lhs_expr, rhs_expr) (                                                          \
    {                                                                                                       \
        auto lhs_value = lhs_expr;                                                                          \
        auto rhs_value = rhs_expr;                                                                          \
                                                                                                            \
        if (!(lhs_value >= rhs_value))                                                                      \
        {                                                                                                   \
            panic(#lhs_expr " >= " #rhs_expr ": {} is not greater or equal than {}", lhs_value, rhs_value); \
        }                                                                                                   \
    })

#define assert_lower_than(lhs_expr, rhs_expr) (                                                 \
    {                                                                                           \
        auto lhs_value = lhs_expr;                                                              \
        auto rhs_value = rhs_expr;                                                              \
                                                                                                \
        if (!(lhs_value < rhs_value))                                                           \
        {                                                                                       \
            panic(#lhs_expr " < " #rhs_expr ": {} is not lower than {}", lhs_value, rhs_value); \
        }                                                                                       \
    })

#define assert_lower_equal(lhs_expr, rhs_expr) (                                                          \
    {                                                                                                     \
        auto lhs_value = lhs_expr;                                                                        \
        auto rhs_value = rhs_expr;                                                                        \
                                                                                                          \
        if (!(lhs_value <= rhs_value))                                                                    \
        {                                                                                                 \
            panic(#lhs_expr " <= " #rhs_expr ": {} is not lower or equal than {}", lhs_value, rhs_value); \
        }                                                                                                 \
    })

#define assert_unreachable() ({ panic("We reach the unreachable"); })
