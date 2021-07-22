#pragma once

#include <ansi/ctypes.h>
#include <brutal/base/std.h>
#include <brutal/text/str.h>

typedef int(ScanMatch)(int);

typedef struct
{
    char const *buffer;
    size_t size;
    size_t head;
} Scan;

void scan_init(Scan *self, Str str);

bool scan_end(Scan *self);

char scan_peek(Scan *self, size_t offset);

char scan_curr(Scan *self);

char scan_next(Scan *self);

void scan_next_n(Scan *self, int n);

long scan_next_decimal(Scan *self); // not binary nor hex

bool scan_skip(Scan *self, char c);

bool scan_skip_word(Scan *self, Str word);

Str scan_skip_until(Scan *self, ScanMatch *match);

#define scan_skip_space(scan) scan_skip_until(scan, isspace)

bool scan_eat(Scan *self, ScanMatch *match);
