#include <brutal/alloc.h>
#include <brutal/io.h>
#include <brutal/log.h>
#include <cc/lex.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        log$("bid usage [bid input] [c output]");
        return 0;
    }

    IoFile source_file;
    io_file_open(&source_file, str$(argv[1]));

    IoReader source_file_reader = io_file_reader(&source_file);

    Buffer source_buffer;
    buffer_init(&source_buffer, 512, alloc_global());

    IoWriter source_buffer_writer = buffer_writer(&source_buffer);

    io_copy(&source_file_reader, &source_buffer_writer);

    Scan scan;
    scan_init(&scan, buffer_str(&source_buffer));

    Lex lex = c_lex(&scan, alloc_global());

    if (scan_dump_error(&scan, io_std_err()))
    {
        return -1;
    }

    lex_dump(&lex, c_lex_to_str);

    return 0;
}
