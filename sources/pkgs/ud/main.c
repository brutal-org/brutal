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

void repl()
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
    }
}

int main(int argc, char **argv)
{

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
        repl();
    }

    return 0;
}
