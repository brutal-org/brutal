#include <brutal-debug>

#include "lex.h"

Lex lex(Scan *scan, LexFn *fn, Alloc *alloc)
{
    return lex_tu(scan, fn, alloc, -1);
}

Lex lex_tu(Scan *scan, LexFn *fn, Alloc *alloc, int translation_unit)
{
    Lex self = {};
    vec_init(&self.lexemes, alloc);

    while (!scan_ended(scan))
    {
        scan_begin(scan);
        int begin = scan->head;

        Lexeme l = {
            .type = fn(scan),
            .str = scan_end(scan),
            (SrcRef){
                .translation_unit = translation_unit,
                .begin = begin,
                .end = scan->head,
            },
        };

        if (l.type == LEXEME_INVALID)
        {
            scan_throw(scan, str$("invalid lexeme"), l.str);
            break;
        }

        vec_push(&self.lexemes, l);
    }

    return self;
}

Lex lex_from_lexeme(Lexemes *from)
{
    Lex self = {};
    self.lexemes = *from;

    return self;
}

void lex_deinit(Lex *self)
{
    vec_deinit(&self->lexemes);
}

bool lex_ended(Lex *self)
{
    return !lex_ok(self) || self->head >= self->lexemes.len;
}

Lexeme lex_peek(Lex *self, int offset)
{
    if (self->has_error || self->head + offset >= self->lexemes.len)
    {
        return (Lexeme){
            LEXEME_EOF,
            str$(""),
            (SrcRef){
                .begin = self->head + offset,
                .end = self->head + offset,
                .translation_unit = -1,
            },
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

LexemeType lex_peek_type(Lex *self, int offset)
{
    return lex_peek(self, offset).type;
}

LexemeType lex_curr_type(Lex *self)
{
    return lex_curr(self).type;
}

LexemeType lex_next_type(Lex *self)
{
    return lex_next(self).type;
}

Buf lex_collect(Lex *self, Alloc *alloc)
{
    Buf buf = {};
    buf_init(&buf, 128, alloc);
    vec_foreach(lexeme, &self->lexemes)
    {
        buf_write(&buf, lexeme->str.buf, lexeme->str.len);
    }
    return buf;
}

bool lex_skip_type(Lex *lex, LexemeType type)
{
    if (lex_curr_type(lex) == type)
    {
        lex_next(lex);
        return true;
    }
    else
    {
        return false;
    }
}

void lex_dump(Lex *self, LexToStrFn fn)
{
    vec_foreach_v(l, &self->lexemes)
    {
        log$("({case-param} {})", fn(l.type), l.str);
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

bool lex_expect(Lex *self, LexemeType type)
{
    if (lex_skip_type(self, type))
    {
        return true;
    }
    else
    {
        lex_throw(self, str$("unexpected-lexeme"));
        return false;
    }
}

bool lex_ok(Lex *self)
{
    return !self->has_error;
}

SrcRef clex_src_ref(Lex *lex, int begin, int end)
{
    Lexeme first = lex->lexemes.data[begin];
    Lexeme last = lex->lexemes.data[end];

    SrcRef cref = {
        .begin = first.pos.begin,
        .end = last.pos.end,
        .translation_unit = first.pos.translation_unit,
    };

    return cref;
}
