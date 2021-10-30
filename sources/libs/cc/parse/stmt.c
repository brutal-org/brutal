#include <cc/parse/parser.h>

CStmt cparse_stmt(Lex *lex, Alloc *alloc)
{
    if (lex_skip_type(lex, CLEX_LBRACE))
    {
        cparse_whitespace(lex);

        CStmt block = cstmt_block(alloc);

        while (!lex_ended(lex) && !lex_skip_type(lex, CLEX_RBRACE))
        {
            CStmt stmt = cparse_stmt(lex, alloc);
            cstmt_block_add(&block, stmt);
            cparse_expect_separator(lex, CLEX_SEMICOLON);
        }

        return block;
    }
    else if (lex_skip_type(lex, CLEX_IF))
    {
        cparse_expect_separator(lex, CLEX_LPARENT);

        CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, alloc);

        cparse_expect_separator(lex, CLEX_RPARENT);

        CStmt stmt_true = cparse_stmt(lex, alloc);

        cparse_whitespace(lex);

        if (!lex_skip_type(lex, CLEX_ELSE))
        {
            return cstmt_if(expr, stmt_true, alloc);
        }

        cparse_whitespace(lex);

        CStmt stmt_false = cparse_stmt(lex, alloc);

        return cstmt_if_else(expr, stmt_true, stmt_false, alloc);
    }
    else if (lex_skip_type(lex, CLEX_FOR))
    {
        cparse_expect_separator(lex, CLEX_LPARENT);

        CStmt init_stmt = cparse_stmt(lex, alloc);

        cparse_expect_separator(lex, CLEX_SEMICOLON);

        CExpr cond_expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, alloc);

        cparse_expect_separator(lex, CLEX_SEMICOLON);

        CExpr iter_expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, alloc);

        cparse_expect_separator(lex, CLEX_RPARENT);

        CStmt stmt = cparse_stmt(lex, alloc);

        return cstmt_for(init_stmt, cond_expr, iter_expr, stmt, alloc);
    }
    else if (lex_skip_type(lex, CLEX_WHILE))
    {
        cparse_expect_separator(lex, CLEX_LPARENT);

        CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, alloc);

        cparse_expect_separator(lex, CLEX_RPARENT);

        CStmt stmt = cparse_stmt(lex, alloc);

        return cstmt_while(expr, stmt, alloc);
    }
    else if (lex_skip_type(lex, CLEX_DO))
    {
        cparse_whitespace(lex);

        CStmt stmt = cparse_stmt(lex, alloc);

        cparse_expect_separator(lex, CLEX_WHILE);

        cparse_expect_separator(lex, CLEX_LPARENT);

        CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, alloc);

        cparse_expect_separator(lex, CLEX_RPARENT);

        return cstmt_do(expr, stmt, alloc);
    }
    else if (lex_skip_type(lex, CLEX_SWITCH))
    {
        cparse_expect_separator(lex, CLEX_LPARENT);

        CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, alloc);

        cparse_expect_separator(lex, CLEX_RPARENT);

        CStmt stmt = cparse_stmt(lex, alloc);

        return cstmt_switch(expr, stmt, alloc);
    }
    else if (lex_skip_type(lex, CLEX_RETURN))
    {
        cparse_whitespace(lex);

        CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, alloc);

        return cstmt_return(expr);
    }
    else if (lex_skip_type(lex, CLEX_GOTO))
    {
        cparse_whitespace(lex);

        if (lex_curr_type(lex) == CLEX_IDENT)
        {
            Str label = lex_curr(lex).str;
            lex_next(lex);
            return cstmt_goto(label, alloc);
        }
        else
        {
            lex_expect(lex, CLEX_IDENT);
            return cstmt_empty();
        }
    }
    else if (lex_skip_type(lex, CLEX_CASE))
    {
        cparse_whitespace(lex);

        CExpr expr = cparse_expr(lex, CEXPR_MAX_PRECEDENCE, alloc);

        return cstmt_case(expr);
    }
    else
    {
        return cstmt_empty();
    }
}
