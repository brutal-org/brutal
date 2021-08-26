#include <brutal/log/locked.h>
#include <brutal/parse/lex.h>

Lex lex(Scan *scan, LexFn *fn, Alloc *alloc)
{
    Lex self = {};
    vec_init(&self.lexemes, alloc);

    while (!scan_ended(scan))
    {
        scan_begin(scan);

        LexemeType type = fn(scan);
        Str str = scan_end(scan);
        Lexeme l = {type, scan_end(scan)};

        vec_push(&self.lexemes, l);
    }

    return self;
}

void lex_deinit(Lex *self)
{
    vec_deinit(&self->lexemes);
}

bool lex_end(Lex *self)
{
    return !lex_ok(self) || self->head >= self->lexemes.length;
}

Lexeme lex_peek(Lex *self, int offset)
{
    if (self->head + offset >= self->lexemes.length)
    {
        return (Lexeme){
            LEXEME_EOF,
            str$(""),
        };
    }

    return self->lexemes.data[self->head + offset];
}

Lexeme lex_curr(Lex *self)
{
    return lex_peek(self, 0);
}

Lexeme lex_next(Lex *self)
{
    Lexeme l = lex_curr(self);
    self->head++;
    return l;
}

void lex_dump(Lex *self, LexToStrFn fn)
{
    vec_foreach(l, &self->lexemes)
    {
        log$("({case:param} {})", fn(l.type), l.str);
    }
}

void lex_throw(Lex *self, Str message)
{
    if (self->has_error)
    {
        return;
    }

    self->has_error = true;
    self->error = (LexError){
        .message = message,
        .lexeme = lex_curr(self),
    };
}

bool lex_expect(Lex *self, int lexeme)
{
    if (lex_curr(self).type != lexeme)
    {
        lex_throw(self, str$("unexpected-lexeme"));
        return false;
    }
    else
    {
        return true;
    }
}

bool lex_ok(Lex *self)
{
    return !self->has_error;
}
