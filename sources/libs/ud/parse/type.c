#include <brutal/debug.h>
#include <ud/parse/decl.h>
#include <ud/parse/lexer.h>
#include "brutal/parse/lex.h"
#include "ud/ast/decl.h"
#include "ud/ast/type.h"

UdTupleDecl ud_parse_tuple_decl(Lex *lex, Alloc *alloc)
{
    UdTupleDecl ret = {};

    vec_init(&ret.fields, alloc);

    while (lex_curr(lex).type != UDLEX_RPAREN)
    {
        ud_parse_whitespace(lex);

        UdType type = {.name = lex_curr(lex).str, .type = ud_str_to_type(lex_curr(lex).str)};

        vec_push(&ret.fields, type);

        lex_next(lex);

        ud_parse_whitespace(lex);

        if (lex_peek(lex, 1).type != UDLEX_RPAREN)
        {
            ud_parse_whitespace(lex);

            if (lex_curr(lex).type != UDLEX_RPAREN)
            {
                ud_parse_whitespace(lex);

                ud_expect(lex, UDLEX_COMMA);
            }

            else
            {
                break;
            }
        }

        else
        {
            ud_parse_whitespace(lex);

            lex_next(lex);

            ud_parse_whitespace(lex);
        }

        if (ud_get_error())
        {
            break;
        }
    }

    return ret;
}

UdDecl ud_parse_type_decl(Lex *lex, Alloc *alloc)
{
    UdDecl ret = {};

    ret.type = UD_DECL_TYPE;

    ud_parse_whitespace(lex);

    ret.name = ud_expect(lex, UDLEX_IDENT).str;

    ud_parse_whitespace(lex);

    ud_expect(lex, UDLEX_EQUAL);

    ud_parse_whitespace(lex);

    if (lex_skip_type(lex, UDLEX_LPAREN))
    {
        ret.type_decl.type = UD_TYPE_TUPLE;
        ret.type_decl.tuple = ud_parse_tuple_decl(lex, alloc);
    }

    return ret;
}
