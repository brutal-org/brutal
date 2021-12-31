#include <brutal/debug.h>
#include <ud/ast.h>
#include <ud/parse/lexer.h>
#include <ud/parse/parse.h>

UdAstNode ud_parse_decl(Lex *lex, Alloc *alloc)
{
    UdAstNode ret = {};

    ret.type = UD_NODE_STMT;
    ret.stmt.type = UD_STMT_DECL;
    ret.stmt.decl_.type = UD_DECL_VAR;

    if (lex_expect(lex, UDLEX_LET))
    {
        UdVarDecl *decl = &ret.stmt.decl_.var;

        ud_parse_whitespace(lex);

        if (lex_curr(lex).type == UDLEX_IDENT)
        {

            ret.stmt.decl_.name = str_dup(lex_curr(lex).str, alloc);

            lex_next(lex);
        }

        ud_parse_whitespace(lex);

        if (lex_expect(lex, UDLEX_COLON))
        {
            ud_parse_whitespace(lex);

            ud_expect(lex, UDLEX_IDENT);

            decl->type.name = str_dup(lex_peek(lex, -1).str, alloc);
        }

        else
        {
            decl->type.name = str$("inferred");
        }

        ud_parse_whitespace(lex);

        ud_expect(lex, UDLEX_EQUAL);

        ud_parse_whitespace(lex);

        decl->value = ud_parse_expr(lex, alloc).expr;

        lex_next(lex);

        ud_expect(lex, UDLEX_SEMICOLON);
    }

    return ret;
}
