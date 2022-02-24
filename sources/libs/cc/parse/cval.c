#include <cc/parse/parser.h>

static CVal cparse_cval_char(Lex *lex)
{

    Str from_str = lex_curr(lex).str;
    Str final_string = str_sub(from_str, 1, from_str.len - 1);

    lex_skip_type(lex, CLEX_CHARACTER);
    return cval_string(final_string);
}

static CVal cparse_cval_string(Lex *lex)
{

    Str from_str = lex_curr(lex).str;
    Str final_string = str_sub(from_str, 1, from_str.len - 1);

    lex_skip_type(lex, CLEX_STRING);
    return cval_string(final_string);
}

static CVal cparse_cval_integer(Lex *lex)
{
    long value = 0;

    str_to_int(lex_curr(lex).str, &value);
    lex_skip_type(lex, CLEX_INTEGER);

    return cval_signed(value);
}

CVal cparse_val(Lex *lex)
{
    int begin = lex->head;
    if (lex_curr_type(lex) == CLEX_STRING)
    {
        return with_cref$(cparse_cval_string(lex), begin, lex);
    }

    if (lex_curr_type(lex) == CLEX_CHARACTER)
    {
        return with_cref$(cparse_cval_char(lex), begin, lex);
    }

    if (lex_curr_type(lex) == CLEX_INTEGER)
    {
        return with_cref$(cparse_cval_integer(lex), begin, lex);
    }

    return (CVal){.type = CVAL_INVALID};
}
