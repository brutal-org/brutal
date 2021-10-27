#include <cc/parse/parser.h>
/* 
this function may be over-complicated but here is what it does: 

    it tries to add an array at the last possible place in the ast: 

    if we have 
    int a[5][10]; 

    we should have an ast converted from: 
    array 10: 
        array 5: 
            int 

    to this: 

    array 5: 
        array 10: 
            int 
    
    so when we add an array we try to put it before the last non array entry in the ast. 

    this function also take in account parenthesis.
*/

static void cparse_add_array_postfix(CType* current, int size, Alloc* alloc)
{
    if(current->type != CTYPE_ARRAY)
    {
        *current = ctype_array(*current, size, alloc);
    }
    else 
    {
        cparse_add_array_postfix(current->array_.subtype, size, alloc);
    }
}

CType cparse_declarator_postfix(Lex *lex, CType type, Alloc *alloc)
{
    while (true)
    {
        if (lex_skip_type(lex, CLEX_LBRACE))
        {
            cparse_eat_whitespace(lex);

            type = ctype_func(type, alloc);

            while (lex_skip_type(lex, CLEX_COMMA))
            {
                CType arg_type = cparse_type(lex, alloc);
                arg_type = cparse_declarator(lex, arg_type, alloc);

                ctype_member(&type, nullstr, arg_type, alloc);

                cparse_eat_whitespace(lex);
            }

            lex_expect(lex, CLEX_RBRACE);
        }
        else if (lex_skip_type(lex, CLEX_LBRACKET))
        {
            cparse_eat_whitespace(lex);

            // TODO: support negative 
            if(lex_curr_type(lex) == CLEX_INTEGER)
            {
                long size = scan_str_to_number(lex_curr(lex).str);
                cparse_add_array_postfix(&type, size, alloc);
                lex_next(lex);
                cparse_eat_whitespace(lex);
            }
            else 
            {
                type = ctype_array(type, 0, alloc);
            }
            
            lex_expect(lex, CLEX_RBRACKET);
        }
        else
        {
            return type;
        }
    }
}

CType cparse_declarator(Lex *lex, CType type, Alloc *alloc)
{
    if (lex_skip_type(lex, CLEX_STAR))
    {
        type = ctype_ptr(type, alloc);
        cparse_eat_whitespace(lex);
        type = cparse_declarator(lex, type, alloc);
        cparse_eat_whitespace(lex);
        type = cparse_declarator_postfix(lex, type, alloc);

        return type;
    }
    else if (lex_skip_type(lex, CLEX_LPARENT))
    {
        cparse_eat_whitespace(lex);
        CType inner = cparse_declarator(lex, ctype_tail(), alloc);
        cparse_eat_whitespace(lex);

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

        cparse_eat_whitespace(lex);
    }

    return attr;
}

CDecl cparse_decl(Lex *lex, Alloc *alloc)
{
    if (lex_skip_type(lex, CLEX_TYPEDEF))
    {
        cparse_eat_whitespace(lex);

        CType type = cparse_type(lex, alloc);
        cparse_eat_whitespace(lex);
        type = cparse_declarator(lex, type, alloc);

        return cdecl_type(str$("TODO"), type, alloc);
    }
    else
    {
        CDeclAttr attr = cparse_decl_attr(lex);
        UNUSED(attr);
        panic_todo$("parse cfunc and cvar");
    }
}
