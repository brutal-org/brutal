#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include <json/emit.h>
#include <ud/ast.h>
#include <ud/gen/json.h>
#include <ud/parse/lexer.h>
#include <ud/parse/parse.h>
#include <ud/repl.h>
#include <ud/sema/sema.h>
#include "brutal/io/buf.h"
#include "brutal/parse/lex.h"

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

    Scan scan;

    scan_init(&scan, expr);

    Lex lex = ud_lex(&scan, alloc);

    UdAst ret = ud_parse(&lex, alloc);

    vec_foreach(node, &ret)
    {
        if (!ud_get_error())
        {
            ud_emit_json(ret, emit, alloc);

            emit_fmt(emit, "\n");
        }
    }

    if (!ud_get_error())
    {
        ud_sema_analyze(sema, &ret);
    }

    else
    {
        ud_set_error(false);
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
            if (str_eq(expr, str$("#quit")) || str_eq(expr, str$("#q")))
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

void eval_file(Str str, Emit *emit, Alloc *alloc, UdSema *sema)
{
    eval(str, alloc, emit, sema);
}

int main(int argc, char **argv)
{
    shift(&argc, &argv);

    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    Emit emit;
    emit_init(&emit, io_chan_out());

    UdSema sema;

    ud_sema_init(&sema, base$(&heap));

    bool do_repl = false;

    Str file = nullstr;

    while (argc != 0)
    {
        Str arg = shift(&argc, &argv);

        if (str_eq(arg, str$("--repl")) || str_eq(arg, str$("-r")))
        {
            do_repl = true;
        }

        else
        {
            file = arg;
        }
    }

    if (do_repl)
    {
        repl(&emit, base$(&heap), &sema);
    }

    else
    {
        IoFile source_file;
        UNWRAP_OR_PANIC(io_file_open(&source_file, file), "File not found!");

        IoReader source_file_reader = io_file_reader(&source_file);

        Buf source_buf;
        buf_init(&source_buf, 512, base$(&heap));

        IoWriter source_buf_writer = buf_writer(&source_buf);

        io_copy(&source_file_reader, &source_buf_writer);

        eval_file(buf_str(&source_buf), &emit, base$(&heap), &sema);
    }

    return 0;
}
