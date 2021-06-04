#pragma once

#include <library/base/count.h>
#include <library/base/map.h>
#include <library/base/std.h>
#include <library/base/str.h>
#include <library/io/write.h>

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

struct print_value print_val_signed(long val);

struct print_value print_val_unsigned(unsigned long val);

struct print_value print_val_string(str_t val);

struct print_value print_val_cstring(const char *val);

// clang-format off

#define PRINT_MATCH(__value)                    \
    _Generic((__value),                         \
        long: print_val_signed,                 \
        unsigned long: print_val_unsigned,      \
        char*: print_val_cstring,         \
        struct str: print_val_string)(__value),

// clang-format on

struct write_result print_details(struct writer *writer, str_t format, struct print_value *values, size_t count);

#define print(writer, fmt, args...) \
    print_details(writer, make_str(fmt), (struct print_value[]){MAP(PRINT_MATCH, args)}, COUNT(args))
