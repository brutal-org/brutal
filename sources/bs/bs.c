#include <brutal/alloc.h>
#include <brutal/io.h>
#include <brutal/log.h>
#include <bs/bs.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        log("bs [files]");
        return 0;
    }

    IoFile source_file;
    io_file_open(&source_file, str_cast(argv[1]));

    IoFileReader source_file_reader = io_file_read(&source_file);

    Buffer source_buffer;
    buffer_init(&source_buffer, 512, alloc_global());

    IoBufferWriter source_buffer_writer = io_buffer_write(&source_buffer);

    io_copy(base_cast(&source_file_reader), base_cast(&source_buffer_writer));

    Scan scan;
    scan_init(&scan, buffer_str(&source_buffer));

    BsExpr expr = bs_parse(&scan, alloc_global());

    if (scan_dump_error(&scan, io_std_err()))
    {
        return -1;
    }

    log("File size {}", buffer_str(&source_buffer).len);
    print(io_std_out(), "[CODE] {}", buffer_str(&source_buffer));

    print(io_std_out(), "[AST] ");
    bs_dump(&expr, io_std_out());

    BsExpr env = bs_env_default(alloc_global());

    print(io_std_out(), "\n[ENV] ");
    bs_dump(&env, io_std_out());
    print(io_std_out(), "\n");

    BsExpr result = bs_eval(expr, &env, alloc_global());

    print(io_std_out(), "\n[RES] ");
    bs_dump(&result, io_std_out());

    return 0;
}
