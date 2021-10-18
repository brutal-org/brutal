#pragma once

#include <brutal/alloc/base.h>
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

#define nullstr str$("")

#define is_nullstr(S) (S.len == 0)

void str_rvs(Str str);

Str str_dup(Str const str, Alloc *alloc);

bool str_eq(Str const lhs, Str const rhs);

int str_count(Str const lStr, Str const rStr);

int str_count_chr(Str const str, char chr);

int str_last(Str const lStr, Str const rStr);

int str_last_chr(Str const str, char chr);

int str_first(Str const lStr, Str const rStr);

int str_first_chr(Str const str, char chr);

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
static inline Str str_make_from_cstr8(uint8_t const *cstr) { return (Str){cstr_len((char *)cstr), (char *)cstr}; }
static inline Str str_make_from_str_fix1(StrFix1 const *str_fix) { return (Str){str_fix->len, (char *)str_fix->buffer}; }
static inline Str str_make_from_str_fix8(StrFix8 const *str_fix) { return (Str){str_fix->len, (char *)str_fix->buffer}; }
static inline Str str_make_from_str_fix16(StrFix16 const *str_fix) { return (Str){str_fix->len, (char *)str_fix->buffer}; }
static inline Str str_make_from_str_fix32(StrFix32 const *str_fix) { return (Str){str_fix->len, (char *)str_fix->buffer}; }
static inline Str str_make_from_str_fix64(StrFix64 const *str_fix) { return (Str){str_fix->len, (char *)str_fix->buffer}; }
static inline Str str_make_from_str_fix128(StrFix128 const *str_fix) { return (Str){str_fix->len, (char *)str_fix->buffer}; }

// clang-format off

// Create a new instance of a non owning string.
#define str$(literal)                                \
    _Generic((literal),                              \
        Str              : str_forward,              \
        InlineStr *      : str_make_from_inline_str, \
        char*            : str_make_from_cstr,       \
        char const*      : str_make_from_cstr,       \
        uint8_t*         : str_make_from_cstr8,       \
        uint8_t const*   : str_make_from_cstr8,       \
        StrFix1*         : str_make_from_str_fix1,   \
        StrFix1 const*   : str_make_from_str_fix1,   \
        StrFix8*         : str_make_from_str_fix8,   \
        StrFix8 const*   : str_make_from_str_fix8,   \
        StrFix16*        : str_make_from_str_fix16,  \
        StrFix16 const*  : str_make_from_str_fix16,  \
        StrFix32*        : str_make_from_str_fix32,  \
        StrFix32 const*  : str_make_from_str_fix32,  \
        StrFix64*        : str_make_from_str_fix64,  \
        StrFix64 const*  : str_make_from_str_fix64,  \
        StrFix128*       : str_make_from_str_fix128, \
        StrFix128 const* : str_make_from_str_fix128  \
    )(literal)

#define str_const$(STR)  (Str){sizeof(STR) - 1, STR}

// clang-format on

#define str_n$(n, str) \
    (Str) { n, str }

// Create a new instance of a fix size string.
#define str_fix$(T, str) (                                    \
    {                                                         \
        T dst_str = {};                                       \
        Str src_str = str$(str);                              \
        mem_cpy(dst_str.buffer, src_str.buffer, src_str.len); \
        dst_str.len = src_str.len;                            \
        dst_str;                                              \
    })

#define str_sub(str, start, end) \
    str_n$(end - start, (char *)str.buffer + start)
