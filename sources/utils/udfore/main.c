#include <brutal-debug>
#include <brutal-fmt>
#include <udfore>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        log$("Usages: {} [input]", argv[0]);
        return 0;
    }
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    IoFile source_file;
    UNWRAP_OR_PANIC(io_file_view(&source_file, str$(argv[1])), "File not found!");

    Buf source_buf;
    buf_init(&source_buf, 512, base$(&heap));

    io_copy(io_file_reader(&source_file), buf_writer(&source_buf));

    Scan scan;
    scan_init(&scan, buf_str(&source_buf));

    Lex lex = udlex(&scan, &heap.base);
    lex_dump(&lex, &udlex_to_str);
    return 0;
}
