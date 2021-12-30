#include <brutal/debug.h>
#include <brutal/ds.h>
#include <brutal/io.h>
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

void emit_expr(Emit *emit, Alloc *alloc, UdExpr expr)
{
    switch (expr.type)
    {
    case UD_EXPR_CONSTANT:
        emit_fmt(emit, "{}", str_from_constant(alloc, expr.const_));
        break;

    case UD_EXPR_REFERENCE:
        emit_fmt(emit, "{}", expr.reference);
        break;

    case UD_EXPR_FUNC_CALL:
        emit_fmt(emit, "call(name = {}, params = [", expr.func_call.name);

        int i = 0;

        vec_foreach(param, &expr.func_call.params)
        {
            emit_expr(emit, alloc, *param);

            if (i + 1 != expr.func_call.params.len)
                emit_fmt(emit, ",");
            i++;
        }

        emit_fmt(emit, "])");

        break;

    case UD_EXPR_BINOP:
        break;

    default:
        emit_fmt(emit, "unknown\n");
    }
}

void ud_print_stmt(Emit *emit, Alloc *alloc, UdStmt stmt)
{
    switch (stmt.type)
    {
    case UD_STMT_DECL:
    {
        if (stmt.decl_.type == UD_DECL_VAR)
        {
            emit_fmt(emit, "=== Var declaration ===\n");
            emit_fmt(emit, "\t -> name: {}\n", stmt.decl_.var.name);
            emit_fmt(emit, "\t -> type: {}\n", stmt.decl_.var.type.name);
            emit_fmt(emit, "\t -> value: ");
            emit_expr(emit, alloc, stmt.decl_.var.value);
            emit_fmt(emit, "\n");
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

UdFuncCall ud_parse_func_call(Lex *lex, Alloc *alloc)
{
    UdFuncCall ret = {};

    vec_init(&ret.params, alloc);

    ret.name = lex_curr(lex).str;

    lex_next(lex);

    ud_parse_whitespace(lex);

    lex_next(lex);

    while (lex_curr(lex).type != UDLEX_RPAREN)
    {
        vec_push(&ret.params, ud_parse_expr(lex, alloc).expr);

        if (lex_peek(lex, 1).type != UDLEX_RPAREN)
        {
            lex_next(lex);

            ud_expect(lex, UDLEX_COMMA);
        }

        else
        {
            lex_next(lex);
        }
    }

    return ret;
}

UdAstNode ud_parse_expr(Lex *lex, Alloc *alloc)
{
    UdAstNode ret = {};

    ret.type = UD_NODE_EXPR;

    if (lex_curr(lex).type == UDLEX_INTEGER || lex_curr(lex).type == UDLEX_STRING)
    {
        ret.expr.type = UD_EXPR_CONSTANT;

        ret.expr.const_ = get_val_from_lexeme(lex_curr(lex));
    }

    else if (lex_curr(lex).type == UDLEX_IDENT && lex_peek(lex, 1).type != UDLEX_LPAREN)
    {
        ret.expr.type = UD_EXPR_REFERENCE;
        ret.expr.reference = lex_curr(lex).str;
    }

    else if (lex_curr(lex).type == UDLEX_IDENT && lex_peek(lex, 1).type == UDLEX_LPAREN)
    {
        ret.expr.type = UD_EXPR_FUNC_CALL;
        ret.expr.func_call = ud_parse_func_call(lex, alloc);
    }

    else
    {
        log$("Cannot parse '{}' yet", lex_curr(lex).str);
    }

    return ret;
}

UdAst ud_parse(Lex *lex, Alloc *alloc)
{
    UdAst ret;

    vec_init(&ret, alloc);

    vec_push(&ret, ud_parse_decl(lex, alloc));
    return ret;
}
