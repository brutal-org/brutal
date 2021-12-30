#include <brutal/debug.h>
#include <brutal/ds.h>
#include <embed/log.h>
#include <ud/ast.h>
#include <ud/parse/lexer.h>
#include <ud/parse/parse.h>

Str str_from_constant(Alloc *alloc, UdVal val)
{
    switch (val.type)
    {
    case UD_VAL_INT:
        return str_fmt(alloc, "{}", val.int_);

    case UD_VAL_STR:
        return val.string;

    default:
        return str$("Unknown");
    }

    return str$("");
}
void print_expr(Alloc *alloc, UdExpr expr)
{
    switch (expr.type)
    {
    case UD_EXPR_CONSTANT:
        log$("{}", str_from_constant(alloc, expr.const_));
        break;
    default:
        break;
    }
}

void print_stmt(Alloc *alloc, UdStmt stmt)
{
    switch (stmt.type)
    {
    case UD_STMT_DECL:
    {
        if (stmt.decl_.type == UD_DECL_VAR)
        {
            log$("=== Var definition ===");
            log$("-> name: {}", stmt.decl_.var.name);
            log$("-> type: {}", stmt.decl_.var.type.name);
            log$("-> value: {}", str_from_constant(alloc, stmt.decl_.var.value.const_));
        }
    }

    default:
        break;
    }
}

UdVal get_val_from_lexeme(Lexeme lexeme)
{
    UdVal ret = {};

    switch (lexeme.type)
    {
    case UDLEX_INTEGER:
        ret.type = UD_VAL_INT;
        ret.int_ = str_to_number(lexeme.str);
        break;
    case UDLEX_STRING:
        ret.type = UD_VAL_STR;
        ret.string = lexeme.str;
    }

    return ret;
}

UdAstNode ud_parse_expr(Lex *lex)
{
    UdAstNode ret = {};

    ret.type = UD_NODE_EXPR;
    ret.expr.type = UD_EXPR_CONSTANT;
    ret.expr.const_ = get_val_from_lexeme(lex_curr(lex));

    return ret;
}

bool ud_expect(Lex *lex, LexemeType type)
{
    if (lex_expect(lex, type))
    {
        return true;
    }

    print(host_log_writer(), "Error while parsing: expected '{}' at {}:{}\n", udlex_to_str(type), lex_curr(lex).line, lex_curr(lex).col);

    return false;
}

UdAstNode ud_parse_decl(Lex *lex)
{
    UdAstNode ret = {};

    ret.type = UD_NODE_STMT;
    ret.stmt.type = UD_STMT_DECL;
    ret.stmt.decl_.type = UD_DECL_VAR;

    if (ud_expect(lex, UDLEX_LET))
    {
        lex_next(lex);

        UdVarDecl *decl = &ret.stmt.decl_.var;

        ud_parse_whitespace(lex);

        if (lex_curr(lex).type == UDLEX_IDENT)
        {
            decl->name = lex_curr(lex).str;

            lex_next(lex);
        }

        ud_parse_whitespace(lex);

        if (lex_expect(lex, UDLEX_COLON))
        {
            ud_parse_whitespace(lex);

            ud_expect(lex, UDLEX_IDENT);

            decl->type.name = lex_peek(lex, -1).str;
        }

        else
        {
            decl->type.name = str$("inferred");
        }

        ud_parse_whitespace(lex);

        ud_expect(lex, UDLEX_EQUAL);

        ud_parse_whitespace(lex);

        decl->value = ud_parse_expr(lex).expr;

        lex_next(lex);

        ud_expect(lex, UDLEX_SEMICOLON);
    }

    return ret;
}

UdAst ud_parse(Lex *lex, Alloc *alloc)
{
    UdAst ret;

    vec_init(&ret, alloc);

    vec_push(&ret, ud_parse_decl(lex));

    vec_foreach(node, &ret)
    {
        print_stmt(alloc, node->stmt);
    }

    return ret;
}
