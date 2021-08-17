#pragma once

#include <brutal/base/count.h>
#include <brutal/base/map.h>
#include <brutal/base/std.h>
#include <brutal/io/write.h>
#include <brutal/text/str.h>

enum print_type
{
    PRINT_SIGNED,
    PRINT_UNSIGNED,
    PRINT_STRING,
    PRINT_POINTER,
    PRINT_CHAR,
};

struct print_value
{
    Str name;
    enum print_type type;

    union
    {
        long _signed;
        unsigned long _unsigned;
        Str _string;
        void *_pointer;
        char _char;
    };
};

struct print_args
{
    size_t count;
    struct print_value *values;
};

struct print_value print_val_signed(long);

struct print_value print_val_unsigned(unsigned long);

struct print_value print_val_string(Str);

struct print_value print_val_cstring(char *);

struct print_value print_val_char(char);

struct print_value print_val_pointer(void *);

// clang-format off

#define PRINT_MATCH(VALUE)                    \
    _Generic((VALUE),                         \
        signed char: print_val_signed,        \
        signed short: print_val_signed,       \
        signed int: print_val_signed,         \
        signed long: print_val_signed,        \
	long long: print_val_signed,          \
                                              \
        unsigned char: print_val_unsigned,    \
        unsigned short: print_val_unsigned,   \
        unsigned int: print_val_unsigned,     \
        unsigned long: print_val_unsigned,    \
	unsigned long long: print_val_unsigned, \
        char*: print_val_cstring,             \
        char: print_val_char,                 \
        Str: print_val_string,                \
        void*: print_val_pointer              \
    )(VALUE),

#define PRINT_NAMED(NAME, VALUE)              \
    ({                                        \
       auto print_value = PRINT_MATCH(VALUE); \
       print_value.name = str_cast(NAME);     \
       print_value;                           \
    })

// clang-format on

#define PRINT_ARGS_(...) \
    (struct print_args) { .count = 0, }

#define PRINT_ARGS_N(...) \
    (struct print_args) { COUNT(__VA_ARGS__), (struct print_value[]){MAP(PRINT_MATCH, __VA_ARGS__)}, }

#define PRINT_ARGS(...) \
    PRINT_ARGS_##__VA_OPT__(N)(__VA_ARGS__)

IoWriteResult print_impl(IoWriter *writer, Str format, struct print_args args);

#define print(writer, fmt, ...) \
    print_impl(writer, str_cast(fmt), PRINT_ARGS(__VA_ARGS__))
