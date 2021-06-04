#include <library/io/scan.h>

void scan_init(struct scan *self, str_t str)
{
    *self = (struct scan){
        .buffer = str.str,
        .size = str.len,
        .head = 0,
    };
}

bool scan_end(struct scan *self)
{
    return self->head + 1 > self->size;
}

char scan_peek(struct scan *self, size_t offset)
{
    if (self->head + offset >= self->size)
    {
        return '\0';
    }

    return self->buffer[self->head + offset];
}

char scan_curr(struct scan *self)
{
    return scan_peek(self, 0);
}

char scan_next(struct scan *self)
{
    char c = scan_peek(self, 0);

    if (!scan_end(self))
    {
        self->head++;
    }

    return c;
}

bool scan_skip(struct scan *self, char c)
{
    if (scan_curr(self) == c)
    {
        scan_next(self);
        return true;
    }
    else
    {
        return false;
    }
}
