#pragma once

#include <library/base/std.h>

struct str
{
    const char *buffer;
    size_t len;
};

typedef struct str str_t;

#define str_fix_t(n) \
    struct           \
    {                \
        char buffer[8]; \
        size_t len;  \
    }

typedef str_fix_t(8) str_fix8_t;
typedef str_fix_t(16) str_fix16_t;
typedef str_fix_t(32) str_fix32_t;
typedef str_fix_t(64) str_fix64_t;
typedef str_fix_t(128) str_fix128_t;

static inline size_t cstr_len(const char *str)
{
    size_t size = 0;

    while (str[size])
    {
        size++;
    }

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
