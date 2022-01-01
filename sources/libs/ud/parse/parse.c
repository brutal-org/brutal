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

void ud_print_expr(Emit *emit, Alloc *alloc, UdExpr expr)
{
    switch (expr.type)
    {
    case UD_EXPR_CONSTANT:
        emit_fmt(emit, "Constant(value={})", str_from_constant(alloc, expr.const_));
        break;

    case UD_EXPR_REFERENCE:
        emit_fmt(emit, "Reference(name={})", expr.reference);
        break;

    case UD_EXPR_FUNC_CALL:
    {
        emit_fmt(emit, "Call(name={}, params=[", expr.func_call.name);

        int i = 0;

        vec_foreach(param, &expr.func_call.params)
        {
            ud_print_expr(emit, alloc, *param);

            if (i + 1 != expr.func_call.params.len)
                emit_fmt(emit, ",");
            i++;
        }

        emit_fmt(emit, "])");

        break;
    }

    case UD_EXPR_BINOP:
        break;

    case UD_EXPR_DECL:
    {
        if (expr.decl.type == UD_DECL_VAR)
        {
            emit_fmt(emit, "VarDecl(name={}, type={}, value=", expr.decl.name, expr.decl.var.type.name);
            ud_print_expr(emit, alloc, *expr.decl.var.value);
            emit_fmt(emit, ")\n");
        }

        if (expr.decl.type == UD_DECL_FUNC)
        {
            emit_fmt(emit, "FuncDecl(name={}, return_type={}, params=[", expr.decl.name, expr.decl.func.return_type.name);

            int i = 0;

            vec_foreach(param, &expr.decl.func.params)
            {
                emit_fmt(emit, "(name={}, type={})", param->name, param->type.name);

                if (i + 1 != expr.decl.func.params.len)
                    emit_fmt(emit, ",");
                i++;
            }

            emit_fmt(emit, "], ");

            emit_fmt(emit, "body=[");

            i = 0;

            vec_foreach(node, &expr.decl.func.body)
            {
                if (i + 1 == expr.decl.func.body.len)
                {
                    emit_fmt(emit, "return=");
                }
                if (!ud_get_error())
                {
                    ud_print_expr(emit, alloc, node->expr);
                }

                i++;
            }

            emit_fmt(emit, "])\n");
        }

        break;
    }

    default:
        emit_fmt(emit, "unknown\n");
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
        ud_parse_whitespace(lex);

        vec_push(&ret.params, ud_parse_expr(lex, alloc).expr);

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
        }

        else
        {
            ud_parse_whitespace(lex);

            lex_next(lex);

            ud_parse_whitespace(lex);
        }
    }

    return ret;
}

// Pure crap
// DON'T USE
UdBinOp ud_parse_bin_op(Lex *lex, Alloc *alloc)
{
    UdBinOp ret = {};

    lex->head -= 1;

    while (lex_curr(lex).type == UDLEX_WHITESPACE)
    {
        lex->head -= 1;
    }

    UdExpr left = ud_parse_expr(lex, alloc).expr;

    log$("{}", left.const_.int_);

    lex_next(lex);
    lex_next(lex);

    ud_parse_whitespace(lex);

    UdExpr right = ud_parse_expr(lex, alloc).expr;

    log$("{}", right.const_.int_);

    ret.left = alloc_malloc(alloc, sizeof(UdExpr));
    ret.right = alloc_malloc(alloc, sizeof(UdExpr));

    mem_cpy(ret.left, &left, sizeof(UdExpr));
    mem_cpy(ret.right, &right, sizeof(UdExpr));

    return ret;
}

UdAstNode ud_parse_expr(Lex *lex, Alloc *alloc)
{
    UdAstNode ret = {};

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

    UdAstNode out = ud_parse_decl(lex, alloc);

    if (out.expr.decl.type == UD_DECL_NONE)
    {
        out = ud_parse_expr(lex, alloc);

        lex_next(lex);

        ud_parse_whitespace(lex);

        if (lex_curr(lex).type == UDLEX_PLUS || lex_curr(lex).type == UDLEX_MINUS || lex_curr(lex).type == UDLEX_STAR || lex_curr(lex).type == UDLEX_SLASH)
        {
            out = ud_parse_expr(lex, alloc);
        }

        ud_expect(lex, UDLEX_SEMICOLON);
    }

    vec_push(&ret, out);

    return ret;
}
