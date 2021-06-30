#include <brutal/io/scan.h>

void scan_init(struct scan *self, Str str)
{
    *self = (struct scan){
        .buffer = str.buffer,
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

long scan_next_decimal(struct scan *self)
{
    long result = 0;
    bool is_negative = false;
    char sign = scan_peek(self, 0);

    if (sign == '-')
    {
        is_negative = true;
        scan_next(self);
    }

    while (!scan_end(self))
    {
        char v = scan_peek(self, 0);
        if (v >= '0' && v <= '9')
        {
            result *= 10;
            result += v - '0';
        }
        else
        {
            if (is_negative)
            {
                result *= -1;
            }
            return result;
        }
        scan_next(self);
    }

    if (is_negative)
    {
        result *= -1;
    }
    return result;
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
