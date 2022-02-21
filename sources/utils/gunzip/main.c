#include <brutal/alloc.h>
#include <brutal/codec/gzip/gzip.h>
#include <brutal/debug.h>
#include <brutal/io.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        log$("Usage: gunzip [OPTION]... [INPUT] [OUTPUT]", argv[0]);
        return -1;
    }

    IoFile source_file;
    UNWRAP_OR_PANIC(io_file_open(&source_file, str$(argv[1]), FILE_OPEN_READ_ONLY), "File not found!");

    IoFile dest_file;
    io_file_create(&dest_file, str$(argv[2]));
    IoResult result = gzip_decompress_stream(io_file_writer(&dest_file), io_file_reader(&source_file));
    if (!result.succ)
    {
        return -1;
    }
    return 0;
}
