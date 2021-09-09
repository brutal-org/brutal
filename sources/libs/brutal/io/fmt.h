#pragma once

#include <brutal/base/defs.h>
#include <brutal/base/ints.h>
#include <brutal/io/write.h>
#include <brutal/parse/scan.h>
#include <brutal/text/case.h>
#include <brutal/text/str.h>

#if COMPILER_GCC
typedef Int128 FmtInt;
typedef UInt128 FmtUInt;
#else
typedef int64_t FmtInt;
typedef uint64_t FmtUInt;
#endif

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

    size_t min_width;
    bool fill_with_zero;
    bool prefix;
} Fmt;

Fmt fmt_parse(Scan *scan);

IoWriteResult fmt_signed(Fmt self, IoWriter *writer, FmtInt value);

IoWriteResult fmt_unsigned(Fmt self, IoWriter *writer, FmtUInt value);

IoWriteResult fmt_string(Fmt self, IoWriter *writer, Str string);

IoWriteResult fmt_char(Fmt self, IoWriter *writer, unsigned int character);
