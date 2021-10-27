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
    char const *buffer;
    int size;
    int head;

    int token;

    bool has_error;
    ScanError error;
} Scan;

void scan_init(Scan *self, Str str);

bool scan_ended(Scan *self);

char scan_peek(Scan *self, int offset);

char scan_curr(Scan *self);

char scan_next(Scan *self);

void scan_next_n(Scan *self, int n);

long scan_next_digit(Scan *self);

long scan_next_decimal(Scan *self);

bool scan_skip(Scan *self, char c);

bool scan_skip_word(Scan *self, Str word);

Str scan_skip_until(Scan *self, ScanMatch *match);

#define scan_skip_space(scan) scan_skip_until(scan, isspace)

bool scan_eat(Scan *self, ScanMatch *match);

void scan_begin(Scan *self);

Str scan_end(Scan *self);

/* --- Error Handeling ------------------------------------------------------ */

void scan_throw(Scan *self, Str message, Str token);

void scan_breakpoint(Scan *self);

bool scan_expect(Scan *self, char c);

bool scan_expect_word(Scan *self, Str word);

bool scan_dump_error(Scan *self, IoWriter *writer);

/* --- Helpers -------------------------------------------------------------- */

long scan_str_to_number(Str string);
