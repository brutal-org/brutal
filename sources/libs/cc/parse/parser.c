#include <cc/parse/parser.h>

CExpr cparse_expr(Lex *lex, Alloc *alloc)
{
    UNUSED(lex);
    UNUSED(alloc);
    panic_todo$("parse cexpr");
}

CStmt cparse_stmt(Lex *lex, Alloc *alloc)
{
    if (lex_skip_type(lex, CLEX_LBRACE))
    {
        cparse_eat_whitespace(lex);

        CStmt block = cstmt_block(alloc);

        while (!lex_ended(lex) && !lex_skip_type(lex, CLEX_RBRACE))
        {
            CStmt stmt = cparse_stmt(lex, alloc);
            cstmt_block_add(&block, stmt);

            cparse_eat_whitespace(lex);
            lex_skip_type(lex, CLEX_SEMICOLON);

            cparse_eat_whitespace(lex);
        }

        return block;
    }
    else if (lex_skip_type(lex, CLEX_IF))
    {
        cparse_eat_whitespace(lex);
        lex_expect(lex, CLEX_LPARENT);
        cparse_eat_whitespace(lex);

        CExpr expr = cparse_expr(lex, alloc);

        cparse_eat_whitespace(lex);
        lex_expect(lex, CLEX_RPARENT);
        cparse_eat_whitespace(lex);

        CStmt stmt_true = cparse_stmt(lex, alloc);

        cparse_eat_whitespace(lex);

        if (!lex_skip_type(lex, CLEX_ELSE))
        {
            return cstmt_if(expr, stmt_true, alloc);
        }

        cparse_eat_whitespace(lex);

        CStmt stmt_false = cparse_stmt(lex, alloc);

        return cstmt_if_else(expr, stmt_true, stmt_false, alloc);
    }
    else if (lex_skip_type(lex, CLEX_FOR))
    {
        cparse_eat_whitespace(lex);
        lex_expect(lex, CLEX_LPARENT);
        cparse_eat_whitespace(lex);

        CStmt init_stmt = cparse_stmt(lex, alloc);

        cparse_eat_whitespace(lex);
        lex_expect(lex, CLEX_SEMICOLON);
        cparse_eat_whitespace(lex);

        CExpr cond_expr = cparse_expr(lex, alloc);

        cparse_eat_whitespace(lex);
        lex_expect(lex, CLEX_SEMICOLON);
        cparse_eat_whitespace(lex);

        CExpr iter_expr = cparse_expr(lex, alloc);

        cparse_eat_whitespace(lex);
        lex_expect(lex, CLEX_RPARENT);
        cparse_eat_whitespace(lex);

        CStmt stmt = cparse_stmt(lex, alloc);

        return cstmt_for(init_stmt, cond_expr, iter_expr, stmt, alloc);
    }
    else if (lex_skip_type(lex, CLEX_WHILE))
    {
        cparse_eat_whitespace(lex);
        lex_expect(lex, CLEX_LPARENT);
        cparse_eat_whitespace(lex);

        CExpr expr = cparse_expr(lex, alloc);

        cparse_eat_whitespace(lex);
        lex_expect(lex, CLEX_RPARENT);
        cparse_eat_whitespace(lex);

        CStmt stmt = cparse_stmt(lex, alloc);

        return cstmt_while(expr, stmt, alloc);
    }
    else if (lex_skip_type(lex, CLEX_DO))
    {
        cparse_eat_whitespace(lex);

        CStmt stmt = cparse_stmt(lex, alloc);

        cparse_eat_whitespace(lex);
        lex_expect(lex, CLEX_WHILE);
        cparse_eat_whitespace(lex);
        lex_expect(lex, CLEX_LPARENT);
        cparse_eat_whitespace(lex);

        CExpr expr = cparse_expr(lex, alloc);

        cparse_eat_whitespace(lex);
        lex_expect(lex, CLEX_RPARENT);
        cparse_eat_whitespace(lex);

        return cstmt_do(expr, stmt, alloc);
    }
    else if (lex_skip_type(lex, CLEX_SWITCH))
    {
        cparse_eat_whitespace(lex);
        lex_expect(lex, CLEX_LPARENT);
        cparse_eat_whitespace(lex);

        CExpr expr = cparse_expr(lex, alloc);

        cparse_eat_whitespace(lex);
        lex_expect(lex, CLEX_RPARENT);
        cparse_eat_whitespace(lex);

        CStmt stmt = cparse_stmt(lex, alloc);

        return cstmt_switch(expr, stmt, alloc);
    }
    else if (lex_skip_type(lex, CLEX_RETURN))
    {
        cparse_eat_whitespace(lex);

        CExpr expr = cparse_expr(lex, alloc);

        return cstmt_return(expr);
    }
    else if (lex_skip_type(lex, CLEX_GOTO))
    {
        cparse_eat_whitespace(lex);

        if (lex_curr_type(lex) == CLEX_ATOM)
        {
            Str label = lex_curr(lex).str;
            lex_next(lex);
            return cstmt_goto(label, alloc);
        }
        else
        {
            lex_expect(lex, CLEX_ATOM);
            return cstmt_empty();
        }
    }
    else if (lex_skip_type(lex, CLEX_CASE))
    {
        cparse_eat_whitespace(lex);

        CExpr expr = cparse_expr(lex, alloc);

        return cstmt_case(expr);
    }
    else
    {
        return cstmt_empty();
    }
}

CUnit cparse_unit(Lex *lex, Alloc *alloc)
{
    CUnit unit = cunit(alloc);

    cparse_eat_whitespace(lex);

    while (!lex_ended(lex))
    {
        cunit_member(&unit, cunit_decl(cparse_decl(lex, alloc)));
        cparse_eat_whitespace(lex);
        lex_skip_type(lex, CLEX_SEMICOLON);
        cparse_eat_whitespace(lex);
    }

    return unit;
}

/*
CVal cparse_val(Lex *lex, Alloc *alloc)
{
}
*/
