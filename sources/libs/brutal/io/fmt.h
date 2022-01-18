#pragma once

#include <brutal/io/traits.h>
#include <brutal/parse/scan.h>
#include <brutal/text/case.h>
#include <brutal/text/str.h>

typedef int64_t FmtInt;
typedef uint64_t FmtUInt;

typedef enum
{
    FMT_NONE,

    FMT_CHAR,
    FMT_STRING,
    FMT_BINARY,
    FMT_OCTAL,
    FMT_DECIMAL,
    FMT_HEXADECIMAL,
    FMT_POINTER,
} FmtType;

typedef struct
{
    FmtType type;
    Case casing;

    long precison;
    long min_width;
    bool fill_with_zero;
    bool prefix;
} Fmt;

Fmt fmt_parse(Scan *scan);

IoResult fmt_signed(Fmt self, IoWriter writer, FmtInt value);

IoResult fmt_unsigned(Fmt self, IoWriter writer, FmtUInt value);

IoResult fmt_float(Fmt self, IoWriter writer, double value);

IoResult fmt_string(Fmt self, IoWriter writer, Str string);

IoResult fmt_char(Fmt self, IoWriter writer, unsigned int character);
