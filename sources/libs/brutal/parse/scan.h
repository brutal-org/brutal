#pragma once

#include <brutal/base/std.h>
#include <brutal/io/write.h>
#include <brutal/text/str.h>
#include <ctype.h>

typedef int(ScanMatch)(int);

typedef struct
{
    int position;
    Str message;
    StrFix128 token;
} ScanError;

typedef struct
{
    char const *buf;
    int size;
    int head;

    int token;

    bool has_error;
    ScanError error;
} Scan;

#define scan_assert_no_error(SCAN)                              \
    ({                                                          \
        if ((SCAN)->has_error)                                  \
        {                                                       \
            scan_dump_error(SCAN, io_chan_out());               \
            panic$("scan: terminating due to previous errors"); \
        }                                                       \
    })

void scan_init(Scan *self, Str str);

bool scan_ended(Scan *self);

char scan_peek(Scan *self, int offset);

char scan_peek_is_any(Scan *self, int offset, Str chars);

char scan_curr(Scan *self);

char scan_curr_is_any(Scan *self, Str chars);

char scan_next(Scan *self);

void scan_next_n(Scan *self, int n);

bool scan_skip(Scan *self, char c);

bool scan_skip_word(Scan *self, Str word);

bool scan_skip_any(Scan *self, Str chars);

Str scan_skip_until(Scan *self, ScanMatch *match);

bool scan_skip_space(Scan *self);

bool scan_eat(Scan *self, ScanMatch *match);

bool scan_eat_any(Scan *self, Str chars);

void scan_begin(Scan *self);

Str scan_end(Scan *self);

/* --- Error Handeling ------------------------------------------------------ */

void scan_throw(Scan *self, Str message, Str token);

void scan_breakpoint(Scan *self);

bool scan_expect(Scan *self, char c);

bool scan_expect_word(Scan *self, Str word);

bool scan_expect_any(Scan *self, Str chars);

bool scan_dump_error(Scan *self, IoWriter *writer);
