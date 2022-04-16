#include <cc/parse/parser.h>
#include <brutal-debug>

CStmt cparse_stmt(Lex *lex, CUnit *context, Alloc *alloc)
{
    if (cparse_skip_separator(lex, CLEX_LBRACE))
    {
        CStmt block = cstmt_block(alloc);

        while (!lex_ended(lex) && !lex_skip_type(lex, CLEX_RBRACE))
        {
            CStmt stmt = cparse_stmt(lex, context, alloc);
            cstmt_block_add(&block, stmt);
            cparse_skip_separator(lex, CLEX_SEMICOLON);
        }

        return block;
    }
    else if (cparse_skip_separator(lex, CLEX_IF))
    {
        cparse_expect_separator(lex, CLEX_LPARENT);
        CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc);
        cparse_expect_separator(lex, CLEX_RPARENT);

        CStmt stmt_true = cparse_stmt(lex, context, alloc);

        if (!cparse_skip_separator(lex, CLEX_ELSE))
        {
            return cstmt_if(expr, stmt_true, alloc);
        }

        CStmt stmt_false = cparse_stmt(lex, context, alloc);

        return cstmt_if_else(expr, stmt_true, stmt_false, alloc);
    }
    else if (cparse_skip_separator(lex, CLEX_FOR))
    {
        cparse_expect_separator(lex, CLEX_LPARENT);

        CStmt init_stmt = cparse_stmt(lex, context, alloc);

        cparse_expect_separator(lex, CLEX_SEMICOLON);

        CExpr cond_expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc);

        cparse_expect_separator(lex, CLEX_SEMICOLON);

        CExpr iter_expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc);

        cparse_expect_separator(lex, CLEX_RPARENT);

        CStmt stmt = cparse_stmt(lex, context, alloc);

        return cstmt_for(init_stmt, cond_expr, iter_expr, stmt, alloc);
    }
    else if (cparse_skip_separator(lex, CLEX_WHILE))
    {
        cparse_expect_separator(lex, CLEX_LPARENT);

        CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc);

        cparse_expect_separator(lex, CLEX_RPARENT);

        CStmt stmt = cparse_stmt(lex, context, alloc);

        return cstmt_while(expr, stmt, alloc);
    }
    else if (cparse_skip_separator(lex, CLEX_DO))
    {
        CStmt stmt = cparse_stmt(lex, context, alloc);

        cparse_expect_separator(lex, CLEX_WHILE);

        cparse_expect_separator(lex, CLEX_LPARENT);

        CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc);

        cparse_expect_separator(lex, CLEX_RPARENT);

        return cstmt_do(expr, stmt, alloc);
    }
    else if (cparse_skip_separator(lex, CLEX_SWITCH))
    {
        cparse_expect_separator(lex, CLEX_LPARENT);

        CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc);

        cparse_expect_separator(lex, CLEX_RPARENT);

        CStmt stmt = cparse_stmt(lex, context, alloc);

        return cstmt_switch(expr, stmt, alloc);
    }
    else if (cparse_skip_separator(lex, CLEX_RETURN))
    {
        CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc);
        return cstmt_return(expr);
    }
    else if (cparse_skip_separator(lex, CLEX_GOTO))
    {
        if (lex_curr_type(lex) == CLEX_IDENT)
        {
            Str label = lex_curr(lex).str;
            lex_next(lex);
            return cstmt_goto(label);
        }
        else
        {
            lex_expect(lex, CLEX_IDENT);
            return cstmt_empty();
        }
    }
    else if (cparse_skip_separator(lex, CLEX_CASE))
    {
        CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc);
        return cstmt_case(expr);
    }
    else if (cparse_is_separator(lex, CLEX_IDENT))
    {
        Str ident = lex_curr(lex).str;
        if (cunit_contains_type(context, ident))
        {
            return cstmt_decl(cparse_decl(lex, context, alloc), alloc);
        }
        else
        {
            return cstmt_expr(cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc));
        }
    }
    else
    {
        return cstmt_expr(cparse_expr(lex, CEXPR_MAX_PRECEDENCE, context, alloc));
    }
}
