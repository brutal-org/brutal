#pragma once

#include <library/base/std.h>

struct str
{
    const char *str;
    size_t len;
};

typedef struct str str_t;

typedef char str16_t[16];
typedef char str32_t[32];
typedef char str64_t[64];
typedef char str128_t[128];

static inline size_t cstr_len(const char *str)
{
    size_t size = 0;
    while (str[size])
        size++;
    return size;
}

static inline str_t str_forward(str_t str)
{
    return str;
}

static inline str_t str_make_from_cstr(const char *cstr)
{
    return (str_t){cstr, cstr_len(cstr)};
}

// clang-format off

#define make_str(literal)                \
    _Generic((literal),                  \
        str_t: str_forward,              \
        char*: str_make_from_cstr,       \
        const char*: str_make_from_cstr) \
        (literal)

// clang-format on
