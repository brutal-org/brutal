#pragma once

#include <library/base/count.h>
#include <library/base/map.h>
#include <library/base/std.h>
#include <library/io/write.h>
#include <library/text/str.h>

enum print_type
{
    PRINT_SIGNED,
    PRINT_UNSIGNED,
    PRINT_STRING,
};

struct print_value
{
    enum print_type type;

    union
    {
        long _signed;
        unsigned long _unsigned;
        str_t _string;
    };
};

struct print_args
{
    size_t count;
    struct print_value *values;
};

struct print_value print_val_signed(long val);

struct print_value print_val_unsigned(unsigned long val);

struct print_value print_val_string(str_t val);

struct print_value print_val_cstring(char *val);

// clang-format off

#define PRINT_MATCH(value)                    \
    _Generic((value),                         \
        signed char: print_val_signed,        \
        signed int: print_val_signed,         \
        signed long: print_val_signed,        \
                                              \
        unsigned char: print_val_unsigned,    \
        unsigned int: print_val_unsigned,     \
        unsigned long: print_val_unsigned,    \
                                              \
        char*: print_val_cstring,             \
        struct str: print_val_string          \
    )(value),

// clang-format on

#define PRINT_ARGS_(...) \
    (struct print_args) { 0, (struct print_value[]){}, }

#define PRINT_ARGS_N(...) \
    (struct print_args) { COUNT(__VA_ARGS__), (struct print_value[]){MAP(PRINT_MATCH, __VA_ARGS__)}, }

#define PRINT_ARGS(...) \
    PRINT_ARGS_##__VA_OPT__(N)(__VA_ARGS__)

write_result_t print_impl(struct writer *writer, str_t format, struct print_args args);

#define print(writer, fmt, ...) \
    print_impl(writer, make_str(fmt), PRINT_ARGS(__VA_ARGS__))
