#pragma once

#include <brutal/base/count.h>
#include <brutal/base/ints.h>
#include <brutal/base/map.h>
#include <brutal/base/std.h>
#include <brutal/io/buffer.h>
#include <brutal/io/fmt.h>
#include <brutal/io/write.h>
#include <brutal/text/str.h>

typedef enum
{
    PRINT_SIGNED,
    PRINT_UNSIGNED,
    PRINT_STRING,
    PRINT_POINTER,
    PRINT_CHAR,
    PRINT_TRANS,
} PrintType;

typedef struct print_value PrintValue;

typedef struct
{
    size_t count;
    PrintValue *values;
} PrintArgs;

typedef struct
{
    Str fmt;
    PrintArgs args;
} PrintTrans;

struct print_value
{
    Str name;
    PrintType type;

    union
    {
        FmtInt _signed;
        FmtUInt _unsigned;
        Str _string;
        void *_pointer;
        char _char;
        PrintTrans _trans;
    };
};

PrintValue print_val_signed(FmtInt);

PrintValue print_val_unsigned(FmtUInt);

PrintValue print_val_string(Str);

PrintValue print_val_cstring(char const *);

PrintValue print_val_char(char);

PrintValue print_val_pointer(void *);

PrintValue print_val_trans(PrintTrans);

// clang-format off

#define PRINT_MATCH(VALUE)                      \
    _Generic((VALUE),                           \
        signed char: print_val_signed,          \
        signed short: print_val_signed,         \
        signed int: print_val_signed,           \
        signed long: print_val_signed,          \
	    signed long long: print_val_signed,     \
                                                \
        unsigned char: print_val_unsigned,      \
        unsigned short: print_val_unsigned,     \
        unsigned int: print_val_unsigned,       \
        unsigned long: print_val_unsigned,      \
	    unsigned long long: print_val_unsigned, \
        char*: print_val_cstring,               \
        char const*: print_val_cstring,         \
        char: print_val_char,                   \
        Str: print_val_string,                  \
        PrintTrans: print_val_trans,            \
        void*: print_val_pointer                \
    )(VALUE),

#define named$(NAME, VALUE) ({                  \
       PrintValue pv = PRINT_MATCH(VALUE);            \
       pv.name = str$(NAME);                    \
       pv;                                      \
    })

// clang-format on

#define PRINT_ARGS_(...) \
    (PrintArgs) { .count = 0, }

#define PRINT_ARGS_N(...) \
    (PrintArgs) { COUNT(__VA_ARGS__), (PrintValue[]){MAP(PRINT_MATCH, __VA_ARGS__)}, }

#define PRINT_ARGS(...) \
    PRINT_ARGS_##__VA_OPT__(N)(__VA_ARGS__)

IoWriteResult print_impl(IoWriter *writer, Str format, PrintArgs args);

#define print(writer, fmt, ...) \
    print_impl(writer, str$(fmt), PRINT_ARGS(__VA_ARGS__))

#define print_trans(_fmt, ...)              \
    (PrintTrans)                            \
    {                                       \
        str$(_fmt), PRINT_ARGS(__VA_ARGS__) \
    }

#define empty_trans() \
    print_trans("")

Str fmt_str_impl(Alloc *alloc, PrintTrans trans);

#define str_fmt(alloc, _fmt, ...) \
    fmt_str_impl(alloc, print_trans((_fmt), __VA_ARGS__))
