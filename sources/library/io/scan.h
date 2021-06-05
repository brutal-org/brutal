#pragma once

#include <library/base/std.h>
#include <library/text/str.h>

struct scan
{
    char const *buffer;
    size_t size;
    size_t head;
};

void scan_init(struct scan *self, str_t str);

bool scan_end(struct scan *self);

char scan_peek(struct scan *self, size_t offset);

char scan_curr(struct scan *self);

char scan_next(struct scan *self);

bool scan_skip(struct scan *self, char c);
