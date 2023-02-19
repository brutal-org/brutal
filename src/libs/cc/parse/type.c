#include <cc/parse/parser.h>

CType cparse_compound_type(Lex *lex, bool is_union, Alloc *alloc)
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
    if (lex_skip_type(lex, CLEX_STRUCT))
    {
        return cparse_compound_type(lex, false, alloc);
    }
    else if (lex_skip_type(lex, CLEX_UNION))
    {
        return cparse_compound_type(lex, true, alloc);
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
            return enum_type;
        }

        cparse_whitespace(lex);

        while (!lex_ended(lex) && lex_skip_type(lex, CLEX_SEMICOLON))
        {
            cparse_whitespace(lex);
        }

        cparse_whitespace(lex);

        lex_expect(lex, CLEX_RBRACE);

        return enum_type;
    }
    else if (lex_skip_type(lex, CLEX_VOID))
    {
        return ctype_void();
    }
    else if (lex_skip_type(lex, CLEX_BOOL))
    {
        return ctype_bool();
    }
    else if (lex_curr_type(lex) == CLEX_IDENT)
    {
        Str name = lex_next(lex).str;
        return ctype_ident(name);
    }
    else
    {
        lex_throw(lex, str$("Unexpected token"));
        return ctype_error();
    }
}
