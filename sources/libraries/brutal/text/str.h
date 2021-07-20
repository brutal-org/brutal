#pragma once

#include <brutal/base/macros.h>
#include <brutal/base/std.h>

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

/* --- Inline String Buffer ------------------------------------------------- */

typedef struct
{
    size_t len;
    char buffer[];
} InlineStr;

/* --- Non Owning Strings --------------------------------------------------- */

typedef struct
{
    size_t len;
    char *buffer;
} Str;

#define nullstr str_cast("")

void str_rvs(Str str);

bool str_eq(const Str lhs, const Str rhs);

int str_cnt(const Str lStr, const Str rStr);

int str_last(const Str lStr, const Str rStr);

int str_first(const Str lStr, const Str rStr);

/* --- Fix Size Strings ----------------------------------------------------- */

#define StrFix(N)       \
    struct              \
    {                   \
        size_t len;     \
        char buffer[N]; \
    }

typedef StrFix(1) StrFix1;
typedef StrFix(8) StrFix8;
typedef StrFix(16) StrFix16;
typedef StrFix(32) StrFix32;
typedef StrFix(64) StrFix64;
typedef StrFix(128) StrFix128;

/* --- Cast Between String Types -------------------------------------------- */

static inline Str str_forward(Str str) { return str; }
static inline Str str_make_from_inline_str(InlineStr *str) { return (Str){str->len, str->buffer}; }
static inline Str str_make_from_cstr(char const *cstr) { return (Str){cstr_len(cstr), (char *)cstr}; }
static inline Str str_make_from_str_fix1(StrFix1 *str_fix) { return (Str){str_fix->len, str_fix->buffer}; }
static inline Str str_make_from_str_fix8(StrFix8 *str_fix) { return (Str){str_fix->len, str_fix->buffer}; }
static inline Str str_make_from_str_fix16(StrFix16 *str_fix) { return (Str){str_fix->len, str_fix->buffer}; }
static inline Str str_make_from_str_fix32(StrFix32 *str_fix) { return (Str){str_fix->len, str_fix->buffer}; }
static inline Str str_make_from_str_fix64(StrFix64 *str_fix) { return (Str){str_fix->len, str_fix->buffer}; }
static inline Str str_make_from_str_fix128(StrFix128 *str_fix) { return (Str){str_fix->len, str_fix->buffer}; }

// clang-format off

// Create a new instance of a non owning string.
#define str_cast(literal)                       \
    _Generic((literal),                         \
        Str         : str_forward,              \
        InlineStr * : str_make_from_inline_str, \
        char*       : str_make_from_cstr,       \
        char const* : str_make_from_cstr,       \
        StrFix1*    : str_make_from_str_fix1,   \
        StrFix8*    : str_make_from_str_fix8,   \
        StrFix16*   : str_make_from_str_fix16,  \
        StrFix32*   : str_make_from_str_fix32,  \
        StrFix64*   : str_make_from_str_fix64,  \
        StrFix128*  : str_make_from_str_fix128  \
    )(literal)

// clang-format on

#define str_cast_n(n, str) \
    (Str) { n, str }

// Create a new instance of a fix size string.
#define str_cast_fix(T, str) (                                \
    {                                                         \
        T dst_str = {};                                       \
        auto src_str = str_cast(str);                         \
        mem_cpy(dst_str.buffer, src_str.buffer, src_str.len); \
        dst_str.len = src_str.len;                            \
        dst_str;                                              \
    })

#define str_sub(str, start, end) \
    str_cast_n(end - start, (char *)str.buffer + start)
