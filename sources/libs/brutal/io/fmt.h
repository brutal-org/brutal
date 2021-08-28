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

typedef struct
{
    enum fmt_type type;
    Case casing;

    size_t min_width;
    bool fill_with_zero;
    bool prefix;
} Fmt;

Fmt fmt_parse(Scan *scan);

IoWriteResult fmt_signed(Fmt self, IoWriter *writer, long value);

IoWriteResult fmt_unsigned(Fmt self, IoWriter *writer, unsigned long value);

IoWriteResult fmt_string(Fmt self, IoWriter *writer, Str string);

IoWriteResult fmt_char(Fmt self, IoWriter *writer, unsigned int character);
