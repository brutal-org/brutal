#include <brutal/debug.h>
#include <cc/parse/parser.h>

CType cparse_declarator_postfix(Lex *lex, CType type, Alloc *alloc)
{
    if (lex_skip_type(lex, CLEX_LPARENT))
    {
        cparse_whitespace(lex);

        CType func = ctype_func(ctype_tail(), alloc);

        while (lex_skip_type(lex, CLEX_COMMA))
        {
            CType arg_type = cparse_type(lex, alloc);
            arg_type = cparse_declarator(lex, arg_type, alloc);
            ctype_member(&func, nullstr, arg_type, alloc);

            cparse_whitespace(lex);
        }

        lex_expect(lex, CLEX_RPARENT);

        CType ret = cparse_declarator_postfix(lex, type, alloc);
        return ctype_append(func, ret, alloc);
    }
    else if (lex_skip_type(lex, CLEX_LBRACKET))
    {
        cparse_whitespace(lex);

        long size = CTYPE_ARRAY_UNBOUNDED;

        if (lex_curr_type(lex) == CLEX_INTEGER)
        {
            size = str_to_number(lex_next(lex).str);
        }

        cparse_separator(lex, CLEX_RBRACKET);

        CType inner = cparse_declarator_postfix(lex, type, alloc);

        return ctype_array(inner, size, alloc);
    }
    else
    {
        return type;
    }
}

CType cparse_declarator(Lex *lex, CType type, Alloc *alloc)
{
    if (lex_skip_type(lex, CLEX_STAR))
    {
        type = ctype_ptr(type, alloc);
        cparse_whitespace(lex);
        type = cparse_declarator(lex, type, alloc);
        cparse_whitespace(lex);
        type = cparse_declarator_postfix(lex, type, alloc);

        return type;
    }
    else if (lex_skip_type(lex, CLEX_LPARENT))
    {
        cparse_whitespace(lex);
        CType inner = cparse_declarator(lex, ctype_tail(), alloc);
        cparse_whitespace(lex);

        lex_expect(lex, CLEX_RPARENT);

        type = ctype_parent(cparse_declarator_postfix(lex, type, alloc), alloc);
        return ctype_append(inner, type, alloc);
    }
    else
    {
        return type;
    }
}

CDeclAttr cparse_decl_attr(Lex *lex)
{
    CDeclAttr attr = 0;

    while (!lex_ended(lex))
    {
        if (lex_skip_type(lex, CLEX_AUTO))
        {
            attr |= CDECL_AUTO;
        }
        else if (lex_skip_type(lex, CLEX_STATIC))
        {
            attr |= CDECL_STATIC;
        }
        else if (lex_skip_type(lex, CLEX_REGISTER))
        {
            attr |= CDECL_REGISTER;
        }
        else if (lex_skip_type(lex, CLEX_INLINE))
        {
            attr |= CDECL_INLINE;
        }
        else if (lex_skip_type(lex, CLEX_EXTERN))
        {
            attr |= CDECL_EXTERN;
        }
        else if (lex_skip_type(lex, CLEX_THREAD_LOCAL))
        {
            attr |= CDECL_THREAD;
        }
        else if (lex_skip_type(lex, CLEX_NORETURN))
        {
            attr |= CDECL_NO_RETURN;
        }
        else
        {
            break;
        }

        cparse_whitespace(lex);
    }

    return attr;
}

CDecl cparse_decl(Lex *lex, Alloc *alloc)
{
    if (lex_skip_type(lex, CLEX_TYPEDEF))
    {
        cparse_whitespace(lex);

        CType type = cparse_type(lex, alloc);
        cparse_whitespace(lex);
        type = cparse_declarator(lex, type, alloc);

        return cdecl_type(str$("TODO"), type, alloc);
    }
    else
    {
        CDeclAttr attr = cparse_decl_attr(lex);
        UNUSED(attr);
        panic$("parse cfunc and cvar not implemented");
    }
}
