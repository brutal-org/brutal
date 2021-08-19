#include <brutal/alloc.h>
#include <brutal/io.h>
#include <brutal/log.h>
#include <udfore/udfore.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        log("{} [files]", str$(argv[0]));
        return 0;
    }

    IoFile source_file;
    io_file_open(&source_file, str$(argv[1]));

    IoFileReader source_file_reader = io_file_read(&source_file);

    Buffer source_buffer;
    buffer_init(&source_buffer, 512, alloc_global());

    IoBufferWriter source_buffer_writer = io_buffer_write(&source_buffer);

    io_copy(base$(&source_file_reader), base$(&source_buffer_writer));

    Scan scan;
    scan_init(&scan, buffer_str(&source_buffer));

    UdExpr expr = ud_parse(&scan, alloc_global());

    if (scan_dump_error(&scan, io_std_err()))
    {
        return -1;
    }

    print(io_std_out(), "[AST] ");
    ud_dump(&expr, io_std_out());

    UdExpr env = ud_env_default(alloc_global());

    print(io_std_out(), "\n[ENV] ");
    ud_dump(&env, io_std_out());
    print(io_std_out(), "\n");

    UdExpr result = ud_eval(expr, &env, alloc_global());

    print(io_std_out(), "\n[RES] ");
    ud_dump(&result, io_std_out());

    return 0;
}
