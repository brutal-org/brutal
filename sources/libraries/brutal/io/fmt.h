#pragma once

#include <brutal/io/scan.h>
#include <brutal/io/write.h>
#include <brutal/text/str.h>

enum fmt_type
{
    FMT_CHAR,
    FMT_STRING,
    FMT_BINARY,
    FMT_OCTAL,
    FMT_DECIMAL,
    FMT_HEXADECIMAL,
    FMT_POINTER,
};

enum fmt_flags
{
    FMT_FLAG_FILL_WITH_ZERO = (1 << 0),   // 0
    FMT_FLAG_USE_PREFIX = (1 << 1),       // #
    FMT_FLAG_FORCE_UPPER_CASE = (1 << 2), // U
    FMT_FLAG_FORCE_LOWER_CASE = (1 << 3), // u
    FMT_FLAG_FORCE_SNAKE_CASE = (1 << 4), // snake_case
    FMT_FLAG_FORCE_CAMEL_CASE = (1 << 5), // CamelCase
};

// any flag that can change how fmt_string work, such as snake_case CamelCase u U ...
#define FMT_STRING_MODIFICATION_FLAGS (FMT_FLAG_FORCE_UPPER_CASE | FMT_FLAG_FORCE_LOWER_CASE | FMT_FLAG_FORCE_SNAKE_CASE | FMT_FLAG_FORCE_CAMEL_CASE)

struct fmt
{
    uint64_t flags;
    enum fmt_type type;
    size_t min_width;
};

struct fmt fmt_parse(Scan *scan);

IoWriteResult fmt_signed(struct fmt self, IoWriter *writer, long value);

IoWriteResult fmt_unsigned(struct fmt self, IoWriter *writer, unsigned long value);

IoWriteResult fmt_string(struct fmt self, IoWriter *writer, Str string);

IoWriteResult fmt_char(struct fmt self, IoWriter *writer, char character);
