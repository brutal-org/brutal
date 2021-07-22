#include <ansi/ctypes.h>
#include <brutal/alloc.h>
#include <brutal/ds.h>
#include <brutal/io/scan.h>

void scan_init(Scan *self, Str str)
{
    *self = (Scan){
        .buffer = str.buffer,
        .size = str.len,
        .head = 0,
    };
}

bool scan_end(Scan *self)
{
    return self->head + 1 > self->size;
}

void scan_scan_skip_space(Scan *self)
{
    while (isspace(scan_curr(self)) && !scan_end(self))
    {
        scan_next(self);
    }
}

Str scan_skip_until(Scan *self, int (*callback)(int))
{
    size_t start = self->head;
    size_t length = 0;
    while (callback(scan_curr(self)) && !scan_end(self))
    {
        length++;
        scan_next(self);
    }

    return str_cast_n(length, (char *)self->buffer + start);
}

char scan_peek(Scan *self, size_t offset)
{
    if (self->head + offset >= self->size)
    {
        return '\0';
    }

    return self->buffer[self->head + offset];
}

char scan_curr(Scan *self)
{
    return scan_peek(self, 0);
}

char scan_next(Scan *self)
{
    char c = scan_peek(self, 0);

    if (!scan_end(self))
    {
        self->head++;
    }

    return c;
}

void scan_next_n(Scan *self, int n)
{
    for (int i = 0; i < n; i++)
    {
        scan_next(self);
    }
}

long scan_next_decimal(Scan *self)
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

bool scan_skip(Scan *self, char c)
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

bool scan_skip_word(Scan *self, Str word)
{
    for (size_t i = 0; i < word.len; i++)
    {
        if (scan_peek(self, i) != word.buffer[i])
        {
            return false;
        }
    }

    scan_next_n(self, word.len);
    return true;
}

bool scan_eat(Scan *self, ScanMatch *match)
{
    bool any = false;

    while (match(scan_curr(self)) && !scan_end(self))
    {
        any = true;
        scan_next(self);
    }

    return any;
}
