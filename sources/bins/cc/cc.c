#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include <cc/c2c.h>
#include <cc/gen/dump.h>
#include <cc/parse.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        log$("bid usage [bid input] [c output]");
        return 0;
    }

    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    IoFile source_file;
    UNWRAP_OR_PANIC(io_file_open(&source_file, str$(argv[1])), "File not found!");

    IoReader source_file_reader = io_file_reader(&source_file);

    Buffer source_buffer;
    buffer_init(&source_buffer, 512, base$(&heap));

    IoWriter source_buffer_writer = buffer_writer(&source_buffer);

    io_copy(&source_file_reader, &source_buffer_writer);

    Scan scan;
    scan_init(&scan, buffer_str(&source_buffer));

    Lex lex = clex(&scan, base$(&heap));

    if (scan_dump_error(&scan, io_std_err()))
    {
        return -1;
    }

    CUnit unit = cparse_unit(&lex, base$(&heap));

    Emit emit;
    emit_init(&emit, io_std_out());

    emit_fmt(&emit, "\n");

    emit_fmt(&emit, "--- BEGIN OG CODE ---\n");
    emit_fmt(&emit, "{}\n", buffer_str(&source_buffer));
    emit_fmt(&emit, "--- END OG CODE ---\n");

    emit_fmt(&emit, "\n");
    emit_fmt(&emit, "\n");

    emit_ident_size(&emit, 2);
    emit_fmt(&emit, "--- BEGIN AST ---\n");
    cdump_unit(&emit, unit);
    emit_fmt(&emit, "--- END AST ---\n");

    emit_fmt(&emit, "\n");
    emit_fmt(&emit, "\n");

    emit_ident_size(&emit, 4);

    emit_fmt(&emit, "--- BEGIN CODE ---\n");
    c2c_unit(&emit, unit);
    emit_fmt(&emit, "--- END CODE ---\n");
    emit_fmt(&emit, "\n");

    heap_alloc_deinit(&heap);

    return 0;
}
