#pragma once

#include "count.h"
#include "keywords.h"
#include "macros.h"
#include "map.h"
#include "std.h"
#include "str.h"
#include "vaopt.h"

typedef enum
{
    ANY_BOOL,
    ANY_INT,
    ANY_UINT,
    ANY_FLOAT,
    ANY_STR,
    ANY_PTR,
} AnyType;

typedef struct
{
    AnyType type;

    union
    {
        bool bool_;
        int64_t int_;
        uint64_t uint_;
#ifndef __freestanding__
        double float_;
#endif
        Str str_;
        void *ptr_;
    };
} Any;

static inline bool any_is(Any any, AnyType type)
{
    return any.type == type;
}

static inline bool any_true(Any any)
{
    switch (any.type)
    {
    case ANY_BOOL:
        return any.bool_;

    case ANY_INT:
        return any.int_ != 0;

    case ANY_UINT:
        return any.uint_ != 0;

#ifndef __freestanding__
    case ANY_FLOAT:
        return any.float_ != 0.0;
#endif

    case ANY_STR:
        return str_any(any.str_);

    case ANY_PTR:
        return any.ptr_ != nullptr;

    default:
        return false;
    }
}

static inline bool any_false(Any any)
{
    return !any_true(any);
}

static inline bool any_as_double(Any any, double *n)
{
    switch (any.type)
    {
    case ANY_BOOL:
        *n = any.bool_ ? 1.0 : 0.0;
        return true;

    case ANY_INT:
        *n = any.int_;
        return true;

    case ANY_UINT:
        *n = any.uint_;
        return true;

#ifndef __freestanding__
    case ANY_FLOAT:
        *n = any.float_;
        return true;
#endif

    default:
        return false;
    }
}

static inline bool any_eq(Any lhs, Any rhs)
{
    if (lhs.type != rhs.type)
    {
        return false;
    }

    switch (lhs.type)
    {
    case ANY_BOOL:
        return lhs.bool_ == rhs.bool_;

    case ANY_INT:
        return lhs.int_ == rhs.int_;

    case ANY_UINT:
        return lhs.uint_ == rhs.uint_;

#ifndef __freestanding__

    case ANY_FLOAT:
        return lhs.float_ == rhs.float_;

#endif

    case ANY_STR:
        return str_eq(lhs.str_, rhs.str_);

    case ANY_PTR:
        return lhs.ptr_ == rhs.ptr_;

    default:
        return false;
    }
}

static inline bool any_not_eq(Any lhs, Any rhs)
{
    return !any_eq(lhs, rhs);
}

static inline Any any_any(Any any)
{
    return any;
}

static inline Any any_bool(bool value)
{
    return (Any){.type = ANY_BOOL, .bool_ = value};
}

static inline Any any_int(int64_t i)
{
    return (Any){.type = ANY_INT, .int_ = i};
}

static inline Any any_uint(uint64_t u)
{
    return (Any){.type = ANY_UINT, .uint_ = u};
}

#ifndef __freestanding__

static inline Any any_float(double f)
{
    return (Any){.type = ANY_FLOAT, .float_ = f};
}

#endif

static inline Any any_str(Str s)
{
    return (Any){.type = ANY_STR, .str_ = s};
}

static inline Any any_cstr(char const *s)
{
    return any_str(str$(s));
}

static inline Any any_ptr(void *p)
{
    return (Any){.type = ANY_PTR, .ptr_ = p};
}

// clang-format off

#ifndef __freestanding__

#define any$(EXPR)                \
    _Generic((EXPR),              \
        Any: any_any,             \
        bool: any_bool,           \
        _MatchSigned(any_int),    \
        _MatchUnsigned(any_uint), \
        _MatchFloat(any_float),   \
                                  \
        char*: any_cstr,          \
        char const*: any_cstr,    \
        Str: any_str,             \
                                  \
        default: any_ptr          \
    )(EXPR)

#else

#define any$(EXPR)                \
    _Generic((EXPR),              \
        Any: any_any,             \
        bool: any_bool,           \
        _MatchSigned(any_int),    \
        _MatchUnsigned(any_uint), \
                                  \
        char*: any_cstr,          \
        char const*: any_cstr,    \
        Str: any_str,             \
                                  \
        default: any_ptr          \
    )(EXPR)

#endif

// clang-format on

typedef struct
{
    Any *buf;
    size_t len;
} AnyVa;

#define __any_va_empty$(...) \
    (AnyVa) { .buf = nullptr, .len = 0 }

#define __any_va_impl$(...) \
    (AnyVa) { (Any[]){MAP_LIST(any$, __VA_ARGS__)}, COUNT(__VA_ARGS__) }

#define any_va$(...)  \
    IFNE(__VA_ARGS__) \
    (__any_va_impl$, __any_va_empty$)(__VA_ARGS__)
