#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include <ud/ast.h>
#include <ud/parse/lexer.h>
#include <ud/parse/parse.h>
#include <ud/repl.h>
#include <ud/sema/sema.h>

Str shift(int *argc, char ***argv)
{
    assert_truth(*argc > 0);

    Str result = str$(**argv);

    *argc -= 1;
    *argv += 1;

    return result;
}

void repl_eval_builtin(Str command)
{
    if (str_eq(command, str$("#info")))
    {
        log$("Udfore Professional edition :tm: provided to you by BRUTALCORP :tm: (c)");
    }
}

void eval(Str expr, Alloc *alloc, Emit *emit, UdSema *sema)
{

    (void)emit;

    Scan scan;

    scan_init(&scan, expr);

    Lex lex = ud_lex(&scan, alloc);

    /*for (int i = 0; i < lex.lexemes.len; i++)
    {
        log$("{}", udlex_to_str(lex.lexemes.data[i].type));
    }*/

    UdAst ret = ud_parse(&lex, alloc);

    vec_foreach(node, &ret)
    {
        if (node->type == UD_NODE_STMT && !ud_get_error())
        {
            ud_print_stmt(emit, alloc, node->stmt);
        }

        else if (node->type == UD_NODE_EXPR && !ud_get_error())
        {
            ud_print_expr(emit, alloc, node->expr);
            emit_fmt(emit, "\n");
        }
    }

    if (!ud_get_error())
    {
        ud_sema_analyze(sema, &ret);
    }

    //bvm_eval(nullptr, nullptr, nullptr);
}

void repl(Emit *emit, Alloc *alloc, UdSema *sema)
{
    Str prompt = str$("ud \033[1;32m# \033[0m");

    while (1)
    {
        Str expr = ud_read_line(prompt);

        if (expr.buf[0] == '#')
        {
            if (str_eq(expr, str$("#quit")))
            {
                ud_do_repl_cleanup(expr);
                break;
            }

            repl_eval_builtin(expr);
        }

        else
        {
            eval(expr, alloc, emit, sema);
        }

        ud_do_repl_cleanup(expr);
    }
}

int main(int argc, char **argv)
{

    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    Emit emit;
    emit_init(&emit, io_chan_out());

    UdSema sema;

    ud_sema_init(&sema, base$(&heap));

    bool do_repl = false;

    while (argc != 0)
    {
        Str arg = shift(&argc, &argv);

        if (str_eq(arg, str$("--repl")))
        {
            do_repl = true;
        }
    }

    if (do_repl)
    {
        repl(&emit, base$(&heap), &sema);
    }

    return 0;
}
