#include <cc/parse/parser.h>
#include <brutal-debug>

void cparse_func_params(Lex *lex, CType *type, Alloc *alloc)
{
    if (!cparse_skip_separator(lex, CLEX_RPARENT))
    {
        do
        {
            CDeclarator decl = cparse_declarator(lex, alloc);
            ctype_member(type, decl.name, decl.type);
        } while (cparse_skip_separator(lex, CLEX_COMMA));

        cparse_expect_separator(lex, CLEX_RPARENT);
    }
}

CType cparse_declarator_postfix(Lex *lex, CType type, Alloc *alloc)
{
    if (cparse_skip_separator(lex, CLEX_LPARENT))
    {
        CType func = ctype_func(ctype_tail(), alloc);

        cparse_func_params(lex, &func, alloc);

        CType ret = cparse_declarator_postfix(lex, type, alloc);
        ctype_append(&func, ret);

        return func;
    }
    else if (cparse_skip_separator(lex, CLEX_LBRACKET))
    {
        cparse_whitespace(lex);

        long size = CTYPE_ARRAY_UNBOUNDED;

        if (lex_curr_type(lex) == CLEX_INTEGER)
        {
            str_to_int(lex_next(lex).str, &size);
        }

        cparse_expect_separator(lex, CLEX_RBRACKET);

        CType inner = cparse_declarator_postfix(lex, type, alloc);

        return ctype_array(inner, size, alloc);
    }
    else
    {
        return type;
    }
}

CDeclarator cparse_declarator_prefix(Lex *lex, CType type, Alloc *alloc)
{
    if (cparse_skip_separator(lex, CLEX_STAR))
    {
        type = ctype_ptr(type, alloc);

        CDeclarator inner = cparse_declarator_prefix(lex, type, alloc);
        cparse_whitespace(lex);
        inner.type = cparse_declarator_postfix(lex, inner.type, alloc);

        return inner;
    }
    else if (cparse_skip_separator(lex, CLEX_LPARENT))
    {
        CDeclarator inner = cparse_declarator_prefix(lex, ctype_tail(), alloc);

        if (!cparse_expect_separator(lex, CLEX_RPARENT))
        {
            ctype_append(&inner.type, ctype_error());
            return inner;
        }

        CType postfix = cparse_declarator_postfix(lex, type, alloc);
        ctype_append(&inner.type, ctype_parent(postfix, alloc));

        return inner;
    }
    else if (lex_curr_type(lex) == CLEX_IDENT)
    {
        return (CDeclarator){
            .type = type,
            .name = lex_next(lex).str,
        };
    }
    else
    {
        return (CDeclarator){
            .type = type,
            .name = str$(""),
        };
    }
}

CDeclarator cparse_declarator(Lex *lex, Alloc *alloc)
{
    CType type = cparse_type(lex, alloc);
    cparse_whitespace(lex);
    CDeclarator decl = cparse_declarator_prefix(lex, type, alloc);

    decl.type = cparse_declarator_postfix(lex, decl.type, alloc);
    return decl;
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

CDecl cparse_decl(Lex *lex, CUnit *context, Alloc *alloc)
{
    if (cparse_is_separator(lex, CLEX_SEMICOLON))
    {
        cparse_skip_separator(lex, CLEX_SEMICOLON);
        return cdecl_empty();
    }
    else if (lex_skip_type(lex, CLEX_TYPEDEF))
    {
        cparse_whitespace(lex);
        CDeclarator declarator = cparse_declarator(lex, alloc);

        return cdecl_type(declarator.name, declarator.type);
    }
    else
    {
        CDeclAttr attr = cparse_decl_attr(lex);
        cparse_whitespace(lex);
        CDeclarator decl = cparse_declarator(lex, alloc);

        if (decl.type.type == CTYPE_FUNC)
        {
            if (cparse_is_separator(lex, CLEX_LBRACE))
            {
                CStmt body = cparse_stmt(lex, context, alloc);
                return cdecl_attrib(cdecl_func(decl.name, decl.type, body), attr);
            }
            else
            {
                cparse_expect_separator(lex, CLEX_SEMICOLON);
                return cdecl_attrib(cdecl_func(decl.name, decl.type, cstmt_empty()), attr);
            }
        }
        else
        {
            if (cparse_skip_separator(lex, CLEX_EQUAL))
            {
                CExpr init = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc);
                return cdecl_var(decl.name, decl.type, init);
            }
            else
            {
                cparse_expect_separator(lex, CLEX_SEMICOLON);
                return cdecl_attrib(cdecl_var(decl.name, decl.type, cexpr_empty()), attr);
            }
        }
    }
}
