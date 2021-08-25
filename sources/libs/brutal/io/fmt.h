#pragma once

#include <brutal/io/write.h>
#include <brutal/parse/scan.h>
#include <brutal/text/case.h>
#include <brutal/text/str.h>

enum fmt_type
{
    FMT_NONE,

    FMT_CHAR,
    FMT_STRING,
    FMT_BINARY,
    FMT_OCTAL,
    FMT_DECIMAL,
    FMT_HEXADECIMAL,
    FMT_POINTER,
};

struct fmt
{
    enum fmt_type type;
    Case casing;

    size_t min_width;
    bool fill_with_zero;
    bool prefix;
};

struct fmt fmt_parse(Scan *scan);

IoWriteResult fmt_signed(struct fmt self, IoWriter *writer, long value);

IoWriteResult fmt_unsigned(struct fmt self, IoWriter *writer, unsigned long value);

IoWriteResult fmt_string(struct fmt self, IoWriter *writer, Str string);

IoWriteResult fmt_char(struct fmt self, IoWriter *writer, unsigned int character);
