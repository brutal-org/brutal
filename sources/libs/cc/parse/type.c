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

CType cparse_type(Lex *lex, Alloc *alloc)
{
    int begin = lex->head;
    if (lex_skip_type(lex, CLEX_STRUCT))
    {
        return with_cref$(cparse_compound_type(lex, false, alloc), begin, lex);
    }
    else if (lex_skip_type(lex, CLEX_UNION))
    {
        return with_cref$(cparse_compound_type(lex, true, alloc), begin, lex);
    }
    else if (lex_skip_type(lex, CLEX_ENUM))
    {
        CType enum_type = ctype_enum(alloc);

        cparse_whitespace(lex);

        if (lex_curr(lex).type == CLEX_IDENT)
        {
            Str name = lex_next(lex).str;
            ctype_named(enum_type, name);
            cparse_whitespace(lex);
        }

        if (!lex_skip_type(lex, CLEX_LBRACE))
        {
            return with_cref$(enum_type, begin, lex);
        }

        cparse_whitespace(lex);

        while (!lex_ended(lex) && lex_skip_type(lex, CLEX_SEMICOLON))
        {
            cparse_whitespace(lex);
        }

        cparse_whitespace(lex);

        lex_expect(lex, CLEX_RBRACE);

        return with_cref$(enum_type, begin, lex);
    }
    else if (lex_skip_type(lex, CLEX_VOID))
    {
        return with_cref$(ctype_void(), begin, lex);
    }
    else if (lex_skip_type(lex, CLEX_BOOL))
    {
        return with_cref$(ctype_bool(), begin, lex);
    }
    else if (lex_skip_type(lex, CLEX_INT))
    {
        return with_cref$(ctype_signed(32), begin, lex);
    }
    else if (lex_skip_type(lex, CLEX_CHAR))
    {
        return with_cref$(ctype_signed(8), begin, lex);
    }
    else if (lex_curr_type(lex) == CLEX_IDENT)
    {
        Str name = lex_next(lex).str;
        return with_cref$(ctype_ident(name), begin, lex);
    }
    else
    {
        lex_throw(lex, str$("Unexpected token"));
        return with_cref$(ctype_error(), begin, lex);
    }
}

bool is_cparse_type(Lex *lex, CUnit *context)
{
    switch(lex_curr_type(lex))
    {
        case CLEX_STRUCT:
        case CLEX_UNION:
        case CLEX_ENUM:
        case CLEX_VOID:
        case CLEX_BOOL:
        case CLEX_INT:
        case CLEX_CHAR:
            return true;
        case CLEX_IDENT:
            return cunit_contains_type(context, lex_curr(lex).str);

        default:
            return false;
    }
}
