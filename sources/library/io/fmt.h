#pragma once

#include <library/base/str.h>
#include <library/io/scan.h>
#include <library/io/write.h>

enum fmt_type
{
    FMT_CHAR,
    FMT_STRING,
    FMT_BINARY,
    FMT_OCTAL,
    FMT_DECIMAL,
    FMT_HEXADECIMAL,
};

struct fmt
{
    enum fmt_type type;
};

struct fmt fmt_parse(struct scan *scan);

struct write_result fmt_signed(struct fmt self, struct writer *writer, long value);

struct write_result fmt_unsigned(struct fmt self, struct writer *writer, unsigned long value);

struct write_result fmt_string(struct fmt self, struct writer *writer, str_t string);
