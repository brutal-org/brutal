#include <brutal/alloc.h>
#include <bvm/eval.h>
#include <ud/ast/expr.h>
#include <ud/gen/expr.h>
#include <ud/parse/lexer.h>
#include <ud/parse/parse.h>
#include <ud/repl.h>

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

void eval(Str expr, Alloc *alloc)
{

    Scan scan;

    scan_init(&scan, expr);

    Lex lex = ud_lex(&scan, alloc);

    for (int i = 0; i < lex.lexemes.len; i++)
    {
        log$("{}", udlex_to_str(lex.lexemes.data[i].type));
    }
    //bvm_eval(nullptr, nullptr, nullptr);
}

void repl(Alloc *alloc)
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
            eval(expr, alloc);
        }

        ud_do_repl_cleanup(expr);
    }
}

int main(int argc, char **argv)
{
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

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
        repl(base$(&heap));
    }

    return 0;
}
