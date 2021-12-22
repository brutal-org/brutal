#include <brutal/alloc.h>
#include <brutal/ds.h>
#include <brutal/io/print.h>
#include <brutal/parse/scan.h>

void scan_init(Scan *self, Str str)
{
    *self = (Scan){
        .buf = str.buf,
        .size = str.len,
        .head = 0,
    };
}

bool scan_ended(Scan *self)
{
    return self->head >= self->size || self->has_error;
}

char scan_peek(Scan *self, int offset)
{
    if (self->has_error)
    {
        return '\0';
    }

    if (offset < 0 && -offset > self->head)
    {
        return '\0';
    }

    if (self->head + offset >= self->size)
    {
        return '\0';
    }

    return self->buf[self->head + offset];
}

char scan_next(Scan *self)
{
    if (self->has_error)
    {
        return '\0';
    }

    if (scan_ended(self))
    {
        scan_throw(self, str$("unexpected end of file"), str$("EOF"));
    }

    char c = scan_peek(self, 0);

    if (!scan_ended(self))
    {
        self->head++;
    }

    return c;
}

void scan_scan_skip_space(Scan *self)
{
    while (isspace(scan_curr(self)) && !scan_ended(self))
    {
        scan_next(self);
    }
}

Str scan_skip_until(Scan *self, int (*callback)(int))
{
    int start = self->head;
    int len = 0;

    while (callback(scan_curr(self)) && !scan_ended(self))
    {
        len++;
        scan_next(self);
    }

    return str_n$(len, (char *)self->buf + start);
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
        if (scan_peek(self, i) != word.buf[i])
        {
            return false;
        }
    }

    scan_next_n(self, word.len);
    return true;
}

bool scan_eat(Scan *self, ScanMatch *match)
{
    bool result = false;

    while (match(scan_curr(self)) && !scan_ended(self))
    {
        result = true;
        scan_next(self);
    }

    return result;
}

void scan_begin(Scan *self)
{
    self->token = self->head;
}

Str scan_end(Scan *self)
{
    return str_n$(
        self->head - self->token,
        (char *)self->buf + self->token);
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
        .token = str_fix$(StrFix128, token),
    };
}

void scan_breakpoint(Scan *self)
{
    scan_throw(self, str$("debug"), str$("breakpoint"));
}

bool scan_expect(Scan *self, char c)
{
    if (!scan_skip(self, c))
    {
        scan_throw(self, str$("expected character"), str_n$(1, &c));
        return false;
    }

    return true;
}

bool scan_expect_word(Scan *self, Str word)
{
    if (!scan_skip_word(self, word))
    {
        scan_throw(self, str$("expected token"), word);
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
    Str src = str_n$(self->size, (char *)self->buf);

    print(writer, "error: {}: {}\n", err.message, str$(&err.token));

    int line_number = str_count_chr(str_sub(src, 0, err.position), '\n') + 1;
    int line_start = str_last_chr(str_sub(src, 0, err.position), '\n') + 1;
    int line_end = str_first_chr(str_sub(src, line_start, src.len), '\n') + line_start;

    if (line_end == -1)
    {
        line_end = src.len;
    }

    Str line = str_sub(src, line_start, line_end);
    print(writer, "    :\n");
    print(writer, "{3d} | {}\n", line_number, line);
    print(writer, "    : ");

    for (int i = 0; i < err.position - line_start; i++)
    {
        print(writer, " ");
    }

    print(writer, "^ here\n");

    return true;
}
