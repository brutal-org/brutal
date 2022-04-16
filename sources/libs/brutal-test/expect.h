#pragma once

#include "runner.h"

#define __expect$(LHS, RHS, OP) (                                                       \
    {                                                                                   \
        typeof(LHS) __lhs = LHS;                                                        \
        typeof(RHS) __rhs = RHS;                                                        \
                                                                                        \
        if (UNLIKELY(!(__lhs OP __rhs)))                                                \
            test_expect(test_self(), loc$(), any$(__lhs), any$(__rhs), stringify$(OP)); \
    })

#define expect$(EXPR) \
    __expect$(EXPR, true, ==)

#define expect_truth$(EXPR) \
    __expect$(EXPR, true, ==)

#define expect_falsity$(EXPR) \
    __expect$(EXPR, false, ==)

#define expect_equal$(LHS, RHS) \
    __expect$(LHS, RHS, ==)

#define expect_not_equal$(LHS, RHS) \
    __expect$(LHS, RHS, !=)

#define expect_greater_than$(LHS, RHS) \
    __expect$(LHS, RHS, >)

#define expect_greater_equal$(LHS, RHS) \
    __expect$(LHS, RHS, >=)

#define expect_lower_than$(LHS, RHS) \
    __expect$(LHS, RHS, <)

#define expect_lower_equal$(LHS, RHS) \
    __expect$(LHS, RHS, <=)

#define expect_null$(EXPR) \
    __expect$(EXPR, nullptr, ==)

#define expect_not_null$(EXPR) \
    __expect$(EXPR, nullptr, !=)

#define expect_str_equal$(LHS, RHS) (                                         \
    {                                                                         \
        typeof(LHS) __lhs = LHS;                                              \
        typeof(RHS) __rhs = RHS;                                              \
                                                                              \
        if (UNLIKELY(!str_eq(__lhs, __rhs)))                                  \
            test_expect(test_self(), loc$(), any$(__lhs), any$(__rhs), "=="); \
    })

#define expect_str_not_equal$(LHS, RHS) (                                     \
    {                                                                         \
        typeof(LHS) __lhs = LHS;                                              \
        typeof(RHS) __rhs = RHS;                                              \
                                                                              \
        if (UNLIKELY(str_eq(__lhs, __rhs)))                                   \
            test_expect(test_self(), loc$(), any$(__lhs), any$(__rhs), "!="); \
    })
