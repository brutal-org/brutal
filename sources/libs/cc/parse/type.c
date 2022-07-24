#include <cc/parse/parser.h>

static CType cparse_compound_type(Lex *lex, bool is_union, Alloc *alloc)
{
    CType compound;

    if (is_union)
    {
        compound = ctype_union(alloc);
    }
    else
    {
        compound = ctype_struct(alloc);
    }

    cparse_whitespace(lex);

    if (lex_curr(lex).type == CLEX_IDENT)
    {
        Str name = lex_next(lex).str;
        ctype_named(compound, name);
        cparse_whitespace(lex);
    }

    if (!lex_skip_type(lex, CLEX_LBRACE))
    {
        return compound;
    }

    cparse_whitespace(lex);

    while (!lex_ended(lex) && lex_skip_type(lex, CLEX_SEMICOLON))
    {
        CDeclarator declarator = cparse_declarator(lex, alloc);
        ctype_member(&compound, declarator.name, declarator.type);
        cparse_whitespace(lex);
    }

    cparse_whitespace(lex);

    lex_expect(lex, CLEX_RBRACE);

    return compound;
}

CType cparse_enum_type(Lex *lex, Alloc *alloc)
{
    CType result = ctype_enum(alloc);

    cparse_whitespace(lex);

    if (lex_curr(lex).type == CLEX_IDENT)
    {
        Str name = lex_next(lex).str;
        ctype_named(result, name);
        cparse_whitespace(lex);
    }

    if (!lex_skip_type(lex, CLEX_LBRACE))
    {
        return result;
    }

    cparse_whitespace(lex);

    while (!lex_ended(lex) && lex_skip_type(lex, CLEX_SEMICOLON))
    {
        cparse_whitespace(lex);
    }

    cparse_whitespace(lex);

    lex_expect(lex, CLEX_RBRACE);

    return result;
}

CType cparse_signed_type(Lex *lex)
{
    if (lex_skip_type(lex, CLEX_CHAR))
    {
        return ctype_char();
    }
    else if (lex_skip_type(lex, CLEX_SHORT))
    {
        return ctype_char();
    }
    else if (lex_skip_type(lex, CLEX_INT))
    {
        return ctype_int();
    }
    else if (lex_skip_type(lex, CLEX_LONG))
    {
        if (lex_skip_type(lex, CLEX_LONG))
        {
            return ctype_longlong();
        }
        else
        {
            return ctype_long();
        }
    }
    else
    {
        lex_throw(lex, str$("Unexpected token"));
        return ctype_error();
    }
}

CType cparse_type(Lex *lex, Alloc *alloc)
{
    int begin = lex->head;
    CType result = ctype_error();

    if (lex_skip_type(lex, CLEX_STRUCT))
    {
        result = cparse_compound_type(lex, false, alloc);
    }
    else if (lex_skip_type(lex, CLEX_UNION))
    {
        result = cparse_compound_type(lex, true, alloc);
    }
    else if (lex_skip_type(lex, CLEX_ENUM))
    {
        result = cparse_enum_type(lex, alloc);
    }
    else if (lex_curr_type(lex) == CLEX_IDENT)
    {
        result = ctype_ident(lex_next(lex).str);
    }
    else if (lex_skip_type(lex, CLEX_VOID))
    {
        result = ctype_void();
    }
    else if (lex_skip_type(lex, CLEX_BOOL))
    {
        result = ctype_bool();
    }
    else if (lex_skip_type(lex, CLEX_SIGNED))
    {
        result = cparse_signed_type(lex);
    }
    else if (lex_skip_type(lex, CLEX_UNSIGNED))
    {
        result = ctype_to_unsigned(cparse_signed_type(lex));
    }
    else if (lex_skip_type(lex, CLEX_FLOAT))
    {
        result = ctype_float();
    }
    else if (lex_skip_type(lex, CLEX_DOUBLE))
    {
        result = ctype_double();
    }
    else
    {
        result = cparse_signed_type(lex);
    }

    return with_cref$(result, begin, lex);
}

bool is_cparse_type(Lex *lex, CUnit *context)
{
    switch (lex_curr_type(lex))
    {
    case CLEX_STRUCT:
    case CLEX_UNION:
    case CLEX_ENUM:
    case CLEX_VOID:
    case CLEX_BOOL:

    case CLEX_CHAR:
    case CLEX_SHORT:
    case CLEX_INT:
    case CLEX_LONG:

    case CLEX_UNSIGNED:
    case CLEX_SIGNED:

    case CLEX_FLOAT:
    case CLEX_DOUBLE:
        return true;

    case CLEX_IDENT:
        return cunit_contains_type(context, lex_curr(lex).str);

    default:
        return false;
    }
}
