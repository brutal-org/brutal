#include <cc/parse/parser.h>

static CVal cparse_cval_char(Lex *lex)
{

    Str from_str = lex_curr(lex).str;
    Str final_str = str_sub(from_str, 1, from_str.len - 1);

    lex_skip_type(lex, CLEX_CHARACTER);
    return cval_str(final_str);
}

static CVal cparse_cval_str(Lex *lex)
{

    Str from_str = lex_curr(lex).str;
    Str final_str = str_sub(from_str, 1, from_str.len - 1);

    lex_skip_type(lex, CLEX_STRING);
    return cval_str(final_str);
}

static CVal cparse_cval_integer(Lex *lex)
{
    long value = 0;

    str_to_int(lex_curr(lex).str, &value);
    lex_skip_type(lex, CLEX_NUMBER);

    return cval_int(value);
}

static CVal cparse_cval_float(Lex *lex)
{
    float value = 0;

    str_to_float(lex_curr(lex).str, &value);
    lex_skip_type(lex, CLEX_NUMBER);

    return cval_float(value);
}

CVal cparse_val(Lex *lex)
{
    CVal result = cval_error();

    int begin = lex->head;

    if (lex_curr_type(lex) == CLEX_STRING)
    {
        result = cparse_cval_str(lex);
    }
    else if (lex_curr_type(lex) == CLEX_CHARACTER)
    {
        result = cparse_cval_char(lex);
    }
    else if (lex_curr_type(lex) == CLEX_NUMBER)
    {
        result = cparse_cval_integer(lex);
    }
    else if (lex_curr_type(lex) == CLEX_NUMBER)
    {
        result = cparse_cval_float(lex);
    }

    return with_cref$(result, begin, lex);
}
