#pragma once

#include <library/io/scan.h>
#include <library/io/write.h>
#include <library/text/str.h>

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


struct fmt
{
    enum fmt_type type;
    size_t min_width;
    bool fill_with_zero;
};

struct fmt fmt_parse(struct scan *scan);

write_result_t fmt_signed(struct fmt self, struct writer *writer, long value);

write_result_t fmt_unsigned(struct fmt self, struct writer *writer, unsigned long value);

write_result_t fmt_string(struct fmt self, struct writer *writer, str_t string);
