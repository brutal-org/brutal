#include <brutal-alloc>
#include <brutal-ds>
#include <brutal-fmt>
#include <brutal-io>

#include "scan.h"

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

int scan_tell(Scan *self)
{
    return self->head;
}

void scan_seek(Scan *self, int offset)
{
    self->head = offset;
}

int scan_size(Scan *self)
{
    return self->size;
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

bool scan_peek_is_any(Scan *self, int offset, Str chars)
{
    for (size_t i = 0; i < chars.len; i++)
    {
        if (scan_peek(self, offset) == chars.buf[i])
        {
            return true;
        }
    }

    return false;
}

char scan_curr(Scan *self)
{
    return scan_peek(self, 0);
}

char scan_curr_is_any(Scan *self, Str chars)
{
    return scan_peek_is_any(self, 0, chars);
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

void scan_next_n(Scan *self, int n)
{
    for (int i = 0; i < n; i++)
    {
        scan_next(self);
    }
}

bool scan_skip_space(Scan *self)
{
    return scan_eat(self, isspace);
}

bool scan_skip(Scan *self, char c)
{
    if (scan_curr(self) != c)
    {
        return false;
    }

    scan_next(self);
    return true;
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

bool scan_skip_any(Scan *self, Str chars)
{
    for (size_t i = 0; i < chars.len; i++)
    {
        if (scan_curr(self) == chars.buf[i])
        {
            scan_next(self);
            return true;
        }
    }

    return false;
}

bool scan_skip_match(Scan *self, ScanMatch *match)
{
    if (!match(scan_curr(self)))
    {
        return false;
    }

    scan_next(self);
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

bool scan_eat_any(Scan *self, Str chars)
{
    bool result = false;

    while (scan_skip_any(self, chars))
    {
        result = true;
    }

    return result;
}

Str scan_eat_match(Scan *self, int (*callback)(int))
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

bool scan_expect_any(Scan *self, Str chars)
{
    if (!scan_skip_any(self, chars))
    {
        scan_throw(self, str$("expected any token"), chars);
        return false;
    }

    return true;
}

bool scan_dump_error(Scan *self, IoWriter writer)
{
    if (!self->has_error)
    {
        return false;
    }

    ScanError err = self->error;
    Str src = str_n$(self->size, (char *)self->buf);

    fmt_format$(writer, "error: {}: {}\n", err.message, str$(&err.token));
    fmt_format$(writer, "    :\n");

    int line_number = str_count_chr(str_sub(src, 0, err.position), '\n') + 1;
    int line_start = str_last_chr(str_sub(src, 0, err.position), '\n') + 1;
    int line_end = str_first_chr(str_sub(src, line_start, src.len), '\n');

    if (line_end == -1)
    {
        line_end = src.len;
    }
    else
    {
        line_end += line_start;
    }

    Str line = str_sub(src, line_start, line_end);
    fmt_format$(writer, "{3d} | {}\n", line_number, line);

    fmt_format$(writer, "    : ");

    for (int i = 0; i < err.position - line_start; i++)
    {
        fmt_format$(writer, " ");
    }

    fmt_format$(writer, "^ here\n");

    return true;
}
