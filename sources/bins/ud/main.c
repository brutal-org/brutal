#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include <ud/eval.h>
#include <ud/gen.h>
#include <ud/parse.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        log$("{} [files]", str$(argv[0]));
        return 0;
    }

    IoFile source_file;
    io_file_open(&source_file, str$(argv[1]));

    IoReader source_file_reader = io_file_reader(&source_file);

    Buf source_buf;
    buf_init(&source_buf, 512, alloc_global());

    IoWriter source_buf_writer = buf_writer(&source_buf);

    io_copy(&source_file_reader, &source_buf_writer);

    Scan scan;
    scan_init(&scan, buf_str(&source_buf));

    UdExpr expr = ud_parse(&scan, alloc_global());

    if (scan_dump_error(&scan, io_std_err()))
    {
        return -1;
    }

    print(io_std_out(), "[AST] ");
    ud_gen_dump(&expr, io_std_out());

    UdExpr env = ud_env_default(alloc_global());

    print(io_std_out(), "\n[ENV] ");
    ud_gen_dump(&env, io_std_out());
    print(io_std_out(), "\n");

    UdExpr result = ud_eval(expr, &env, alloc_global());

    print(io_std_out(), "\n[RES] ");
    ud_gen_dump(&result, io_std_out());

    return 0;
}
