#pragma once

#include <brutal-io>
#include <brutal-parse>
#include <brutal-text>

#include "case.h"

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

typedef enum
{
    FMT_PRINTF_INT,
    FMT_PRINTF_LONG,
    FMT_PRINTF_LONGLONG,

    FMT_PRINTF_UINT,
    FMT_PRINTF_ULONG,
    FMT_PRINTF_ULONGLONG,

    FMT_PRINTF_FLOAT,

    FMT_PRINTF_PTR,
    FMT_PRINTF_STRING,
} FmtPrintfType;

typedef struct
{
    FmtType type;
    Case casing;

    long width;
#define FMT_PRECISION_DEFAULT (-1)
    long precision;

    char fill;
    bool prefix;
    bool prefix_plus;
} Fmt;

Fmt fmt_parse(Scan *scan);

FmtPrintfType fmt_parse_printf(Scan *scan, Fmt *fmt);

IoResult fmt_signed(Fmt self, IoWriter writer, int64_t value);

IoResult fmt_unsigned(Fmt self, IoWriter writer, uint64_t value);

#ifndef __freestanding__
IoResult fmt_float(Fmt self, IoWriter writer, double value);
#endif

IoResult fmt_string(Fmt self, IoWriter writer, Str string);

IoResult fmt_char(Fmt self, IoWriter writer, unsigned int character);

IoResult fmt_any(Fmt self, IoWriter writer, Any value);
