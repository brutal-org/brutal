#include <brutal/alloc.h>
#include <brutal/ds.h>
#include <brutal/io/print.h>
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
    return self->head + 1 > self->size || self->has_error;
}

char scan_peek(Scan *self, size_t offset)
{
    if (self->has_error)
    {
        return '\0';
    }

    if (self->head + offset >= self->size)
    {
        return '\0';
    }

    return self->buffer[self->head + offset];
}

char scan_next(Scan *self)
{
    if (self->has_error)
    {
        return '\0';
    }

    if (scan_end(self))
    {
        scan_throw(self, str_cast("unexpected end of file"), str_cast("EOF"));
    }

    char c = scan_peek(self, 0);

    if (!scan_end(self))
    {
        self->head++;
    }

    return c;
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

char scan_curr(Scan *self)
{
    return scan_peek(self, 0);
}

void scan_next_n(Scan *self, int n)
{
    for (int i = 0; i < n; i++)
    {
        scan_next(self);
    }
}

long scan_next_digit(Scan *self)
{
    char c = tolower(scan_next(self));

    if (isalnum(c))
    {
        return c - 'a' + 10;
    }
    else
    {
        return c - '0';
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

void scan_begin_token(Scan *self)
{
    self->token = self->head;
}

Str scan_end_token(Scan *self)
{
    return str_cast_n(
        self->head - self->token,
        (char *)self->buffer + self->token);
}

/* --- Error Handeling ------------------------------------------------------ */

void scan_throw(Scan *self, Str message, Str token)
{
    if (self->has_error)
    {
        return;
    }

    self->has_error = true;
    self->error = (ScanError){
        .message = message,
        .position = self->head,
        .token = str_cast_fix(StrFix128, token),
    };
}

void scan_breakpoint(Scan *self)
{
    scan_throw(self, str_cast("debug"), str_cast("breakpoint"));
}

bool scan_expect(Scan *self, char c)
{
    if (!scan_skip(self, c))
    {
        scan_throw(self, str_cast("expected character"), str_cast_n(1, &c));
        return false;
    }

    return true;
}

bool scan_expect_word(Scan *self, Str word)
{
    if (!scan_skip_word(self, word))
    {
        scan_throw(self, str_cast("expected token"), word);
        return false;
    }

    return true;
}

bool scan_dump_error(Scan *self, IoWriter *writer)
{
    if (!self->has_error)
    {
        return false;
    }

    ScanError err = self->error;
    Str src = str_cast_n(self->size, (char *)self->buffer);

    print(writer, "error: {}: {}\n", err.message, str_cast(&err.token));

    int line_number = str_count_chr(str_sub(src, 0, err.position), '\n') + 1;
    int line_start = str_last_chr(str_sub(src, 0, err.position), '\n') + 1;
    int line_end = str_first_chr(str_sub(src, line_start, src.len), '\n') + line_start;

    Str line = str_sub(src, line_start, line_end);

    print(writer, "    |\n");
    print(writer, "{3d} | {}\n", line_number, line);
    print(writer, "    | ");

    for (int i = 0; i < err.position - line_start; i++)
    {
        print(writer, " ");
    }

    print(writer, "^ here\n");

    return true;
}
