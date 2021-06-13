#pragma once

#include <brutal/base/macros.h>
#include <brutal/base/std.h>
#include <brutal/mem.h>

struct str
{
    char *buffer;
    size_t len;
};

typedef struct str str_t;

#define str_fix_t(N)    \
    struct              \
    {                   \
        char buffer[N]; \
        size_t len;     \
    }

typedef str_fix_t(8) str_fix8_t;
typedef str_fix_t(16) str_fix16_t;
typedef str_fix_t(32) str_fix32_t;
typedef str_fix_t(64) str_fix64_t;
typedef str_fix_t(128) str_fix128_t;

#define nullstr make_str("")

static inline size_t cstr_len(char const *str)
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

static inline str_t str_make_from_cstr(char const *cstr)
{
    return (str_t){(char *)cstr, cstr_len(cstr)};
}

// clang-format off

#define make_str(literal)                \
    _Generic((literal),                  \
        str_t: str_forward,              \
        char*: str_make_from_cstr,       \
        char const*: str_make_from_cstr  \
    )(literal)

#define make_str_fix(T, str)                                  \
    ({                                                        \
        T dst_str;                                            \
        auto src_str = make_str(str);                         \
        mem_cpy(dst_str.buffer, src_str.buffer, src_str.len); \
        dst_str.len = src_str.len;                            \
        dst_str;                                              \
    })

#define make_str_n(str, n) \
    (str_t){str, n}

// clang-format on

void str_rvs(str_t str);

bool str_eq(str_t lhs, str_t rhs);
