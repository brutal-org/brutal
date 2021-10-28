#include <cc/parse/parser.h>

static CVal cparse_cval_char(Lex *lex, Alloc *alloc)
{

    Str from_str = lex_curr(lex).str;
    Str final_string = str_sub(from_str, 1, from_str.len - 1);

    lex_skip_type(lex, CLEX_CHARACTER);
    return cval_string(final_string, alloc);
}

static CVal cparse_cval_string(Lex *lex, Alloc *alloc)
{
    Str from_str = lex_curr(lex).str;
    Str final_string = str_sub(from_str, 1, from_str.len - 1);

    lex_skip_type(lex, CLEX_STRING);
    return cval_string(final_string, alloc);
}

static CVal cparse_cval_integer(Lex *lex)
{
    long value = str_to_number(lex_curr(lex).str);
    lex_skip_type(lex, CLEX_INTEGER);

    return cval_signed(value);
}

CVal cparse_val(Lex *lex, Alloc *alloc)
{
    if (lex_curr_type(lex) == CLEX_STRING)
    {
        return cparse_cval_string(lex, alloc);
    }

    if (lex_curr_type(lex) == CLEX_CHARACTER)
    {
        return cparse_cval_char(lex, alloc);
    }

    if (lex_curr_type(lex) == CLEX_INTEGER)
    {
        return cparse_cval_integer(lex);
    }

    return (CVal){.type = CVAL_INVALID};
}
