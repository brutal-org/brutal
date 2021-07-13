#pragma once

#include <ansi/ctypes.h>
#include <brutal/base/std.h>
#include <brutal/text/str.h>

struct scan
{
    char const *buffer;
    size_t size;
    size_t head;
};

void scan_init(struct scan *self, Str str);

bool scan_end(struct scan *self);

char scan_peek(struct scan *self, size_t offset);

char scan_curr(struct scan *self);

char scan_next(struct scan *self);

long scan_next_decimal(struct scan *self); // not binary nor hex

bool scan_skip(struct scan *self, char c);

Str scan_skip_until(struct scan *self, int (*callback)(int));

#define skip_space(scanner) scan_skip_until(scanner, isspace)

#define scan_alnum(scanner) scan_skip_until(scanner, isalnum)
