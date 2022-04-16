#include <brutal-debug>
#include <codec-gzip>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        log$("Usage: gzip [OPTION]... [INPUT] [OUTPUT]", argv[0]);
        return -1;
    }

    IoFile source_file;
    UNWRAP_OR_PANIC(io_file_view(&source_file, str$(argv[1])), "File not found!");

    IoFile dest_file;
    io_file_create(&dest_file, str$(argv[2]));
    IoResult result = gzip_compress_stream(io_file_writer(&dest_file), io_file_reader(&source_file));
    if (!result.succ)
    {
        return -1;
    }
    return 0;
}
