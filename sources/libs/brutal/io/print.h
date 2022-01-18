#pragma once

#include <brutal/base/count.h>
#include <brutal/base/map.h>
#include <brutal/base/std.h>
#include <brutal/base/vaopt.h>
#include <brutal/io/fmt.h>
#include <brutal/text/str.h>

typedef enum
{
    PRINT_SIGNED,
    PRINT_UNSIGNED,
    PRINT_FLOAT,
    PRINT_STRING,
    PRINT_POINTER,
    PRINT_CHAR,
} PrintType;

typedef struct print_value PrintValue;

typedef struct
{
    size_t count;
    PrintValue *values;
} PrintArgs;

struct print_value
{
    Str name;
    PrintType type;

    union
    {
        FmtInt _signed;
        FmtUInt _unsigned;
        double _float;
        Str _string;
        void *_pointer;
        char _char;
    };
};

PrintValue print_val_signed(FmtInt);

PrintValue print_val_unsigned(FmtUInt);

PrintValue print_val_float(double);

PrintValue print_val_string(Str);

PrintValue print_val_cstring(char const *);

PrintValue print_val_char(char);

PrintValue print_val_pointer(void *);

// clang-format off

#define PRINT_MATCH(VALUE)                      \
    _Generic((VALUE),                           \
        signed char: print_val_signed,          \
        signed short: print_val_signed,         \
        signed int: print_val_signed,           \
        signed long: print_val_signed,          \
	    signed long long: print_val_signed,     \
        float: print_val_float,                 \
        double: print_val_float,                \
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
        void*: print_val_pointer                \
    )(VALUE),

#define named$(NAME, VALUE) ({                  \
       PrintValue pv = PRINT_MATCH(VALUE);      \
       pv.name = str$(NAME);                    \
       pv;                                      \
    })

// clang-format on

#define PRINT_ARGS_(...) \
    (PrintArgs) { .count = 0, }

#define PRINT_ARGS_N(...) \
    (PrintArgs) { COUNT(__VA_ARGS__), (PrintValue[]){MAP(PRINT_MATCH, __VA_ARGS__)}, }

#define PRINT_ARGS(...) \
    IFNE(__VA_ARGS__)   \
    (PRINT_ARGS_N, PRINT_ARGS_)(__VA_ARGS__)

IoResult print_impl(IoWriter writer, Str format, PrintArgs args);

#define print(writer, fmt, ...) \
    print_impl(writer, str$(fmt), PRINT_ARGS(__VA_ARGS__))

Str str_fmt_impl(Alloc *alloc, Str format, PrintArgs args);

#define str_fmt(alloc, fmt, ...) \
    str_fmt_impl(alloc, str$(fmt), PRINT_ARGS(__VA_ARGS__))
