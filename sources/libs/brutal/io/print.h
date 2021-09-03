#pragma once

#include <brutal/base/count.h>
#include <brutal/base/map.h>
#include <brutal/base/std.h>
#include <brutal/io/buffer.h>
#include <brutal/io/write.h>
#include <brutal/text/str.h>

enum print_type
{
    PRINT_SIGNED,
    PRINT_UNSIGNED,
    PRINT_STRING,
    PRINT_POINTER,
    PRINT_CHAR,
    PRINT_TRANS,
};

typedef struct
{
    size_t count;
    struct print_value *values;
} PrintArgs;

typedef struct
{
    Str fmt;
    PrintArgs args;
} PrintTrans;

struct print_value
{
    Str name;
    enum print_type type;

    union
    {
        long _signed;
        unsigned long long _unsigned;
        Str _string;
        void *_pointer;
        char _char;
        PrintTrans _trans;
    };
};

struct print_value print_val_signed(long);

struct print_value print_val_unsigned(unsigned long long);

struct print_value print_val_string(Str);

struct print_value print_val_cstring(char const *);

struct print_value print_val_char(char);

struct print_value print_val_pointer(void *);

struct print_value print_val_trans(PrintTrans);

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
        char const*: print_val_cstring,               \
        char: print_val_char,                   \
        Str: print_val_string,                  \
        PrintTrans: print_val_trans, \
        void*: print_val_pointer                \
    )(VALUE),

#define PRINT_NAMED(NAME, VALUE)                \
    ({                                          \
       auto print_value = PRINT_MATCH(VALUE);   \
       print_value.name = str$(NAME);       \
       print_value;                             \
    })

// clang-format on

#define PRINT_ARGS_(...) \
    (PrintArgs) { .count = 0, }

#define PRINT_ARGS_N(...) \
    (PrintArgs) { COUNT(__VA_ARGS__), (struct print_value[]){MAP(PRINT_MATCH, __VA_ARGS__)}, }

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
