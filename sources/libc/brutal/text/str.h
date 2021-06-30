#pragma once

#include <brutal/base/macros.h>
#include <brutal/base/std.h>
#include <brutal/mem.h>

/* --- Null Terminated String ----------------------------------------------- */

static inline size_t cstr_len(char const *str)
{
    size_t size = 0;

    while (str[size])
    {
        size++;
    }

    return size;
}

/* --- Non Owning Strings --------------------------------------------------- */

typedef struct
{
    char *buffer;
    size_t len;
} Str;

#define nullstr str_cast("")

void str_rvs(Str str);

bool str_eq(Str lhs, Str rhs);

/* --- Fix Size Strings ----------------------------------------------------- */

#define StrFix(N)       \
    struct              \
    {                   \
        char buffer[N]; \
        size_t len;     \
    }

typedef StrFix(8) StrFix8;
typedef StrFix(16) StrFix16;
typedef StrFix(32) StrFix32;
typedef StrFix(64) StrFix64;
typedef StrFix(128) StrFix128;

/* --- Cast Between String Types -------------------------------------------- */

static inline Str str_forward(Str str) { return str; }
static inline Str str_make_from_cstr(char const *cstr) { return (Str){(char *)cstr, cstr_len(cstr)}; }
static inline Str str_make_from_str_fix8(StrFix8 *str_fix) { return (Str){str_fix->buffer, str_fix->len}; }
static inline Str str_make_from_str_fix16(StrFix16 *str_fix) { return (Str){str_fix->buffer, str_fix->len}; }
static inline Str str_make_from_str_fix32(StrFix32 *str_fix) { return (Str){str_fix->buffer, str_fix->len}; }
static inline Str str_make_from_str_fix64(StrFix64 *str_fix) { return (Str){str_fix->buffer, str_fix->len}; }
static inline Str str_make_from_str_fix128(StrFix128 *str_fix) { return (Str){str_fix->buffer, str_fix->len}; }

// clang-format off

// Create a new instance of a non owning string.
#define str_cast(literal)                         \
    _Generic((literal),                           \
        Str         : str_forward,              \
        char*         : str_make_from_cstr,       \
        char const*   : str_make_from_cstr,       \
        StrFix8*   : str_make_from_str_fix8,   \
        StrFix16*  : str_make_from_str_fix16,  \
        StrFix32*  : str_make_from_str_fix32,  \
        StrFix64*  : str_make_from_str_fix64,  \
        StrFix128* : str_make_from_str_fix128  \
    )(literal)

// clang-format on

#define str_cast_n(str, n) \
    (Str) { str, n }

// Create a new instance of a fix size string.
#define str_cast_fix(T, str) (                                \
    {                                                         \
        T dst_str = {};                                       \
        auto src_str = str_cast(str);                         \
        mem_cpy(dst_str.buffer, src_str.buffer, src_str.len); \
        dst_str.len = src_str.len;                            \
        dst_str;                                              \
    })
