#include <cc/trans.h>

static bool ctran_should_stmt_endline(CStmtType type)
{
    if (type == CSTMT_BLOCK ||
        type == CSTMT_DEFAULT ||
        type == CSTMT_LABEL ||
        type == CSTMT_CASE)
    {
        return false;
    }

    return true;
}

void ctran_stmt(Emit *emit, CStmt stmt)
{
    switch (stmt.type)
    {
    case CSTMT_EMPTY:
        break;

    case CSTMT_DECL:
        ctran_decl(emit, *stmt.decl_.decl);
        return;

    case CSTMT_EXPR:
        ctran_expr(emit, stmt.expr_.expr);
        return;

    case CSTMT_BLOCK:
        emit_fmt(emit, "{{\n");
        emit_ident(emit);

        vec_foreach_v(v, &stmt.block_.stmts)
        {
            ctran_stmt(emit, v);
            if (ctran_should_stmt_endline(v.type))
            {
                emit_fmt(emit, ";");
            }
            emit_fmt(emit, "\n");
        }

        emit_deident(emit);
        emit_fmt(emit, "}}");
        return;

    case CSTMT_IF:
        emit_fmt(emit, "if (");

        ctran_expr(emit, stmt.if_.expr);
        emit_fmt(emit, ")\n");

        if (stmt.if_.stmt_true->type != CSTMT_BLOCK)
        {
            emit_ident(emit);
            ctran_stmt(emit, *stmt.if_.stmt_true);
            emit_deident(emit);
        }
        else
        {
            ctran_stmt(emit, *stmt.if_.stmt_true);
        }

        if (stmt.if_.stmt_false->type != CSTMT_EMPTY)
        {
            emit_fmt(emit, "\nelse\n");
            if (stmt.if_.stmt_true->type != CSTMT_BLOCK)
            {
                emit_ident(emit);
                ctran_stmt(emit, *stmt.if_.stmt_false);
                emit_deident(emit);
            }
            else
            {
                ctran_stmt(emit, *stmt.if_.stmt_false);
            }
        }
        return;

    case CSTMT_FOR:
        emit_fmt(emit, "for (");
        ctran_stmt(emit, *stmt.for_.init_stmt);

        emit_fmt(emit, "; ");
        ctran_expr(emit, stmt.for_.cond_expr);

        emit_fmt(emit, "; ");
        ctran_expr(emit, stmt.for_.iter_expr);

        emit_fmt(emit, ")\n");
        ctran_stmt(emit, *stmt.for_.stmt);
        return;

    case CSTMT_WHILE:
        emit_fmt(emit, "while (");
        ctran_expr(emit, stmt.while_.expr);
        emit_fmt(emit, ") \n");

        ctran_stmt(emit, *stmt.while_.stmt);
        return;

    case CSTMT_DO:
        emit_fmt(emit, "do \n ");

        ctran_stmt(emit, *stmt.do_.stmt);

        emit_fmt(emit, "while (");
        ctran_expr(emit, stmt.do_.expr);
        emit_fmt(emit, ") \n");
        return;

    case CSTMT_SWITCH:
        emit_fmt(emit, "switch (");
        ctran_expr(emit, stmt.while_.expr);
        emit_fmt(emit, ")\n");
        ctran_stmt(emit, *stmt.while_.stmt);
        return;

    case CSTMT_RETURN:
        emit_fmt(emit, "return ");
        ctran_expr(emit, stmt.return_.expr);
        return;

    case CSTMT_GOTO:
        emit_fmt(emit, "goto {}", stmt.goto_.label);
        return;

    case CSTMT_BREAK:
        emit_fmt(emit, "break");
        return;

    case CSTMT_CONTINUE:
        emit_fmt(emit, "continue");
        return;

    case CSTMT_LABEL:
        emit_fmt(emit, "{}:", stmt.label_.label);
        return;

    case CSTMT_CASE:
        emit_fmt(emit, "case ");
        ctran_expr(emit, stmt.case_.expr);
        emit_fmt(emit, ":");
        return;

    case CSTMT_DEFAULT:
        emit_fmt(emit, "default:");
        return;

    default:
        return;
    }
}
