#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include <cc/gen.h>
#include <cc/parse.h>
#include <codegen/codegen.h>

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

    Buf source_buf;
    buf_init(&source_buf, 512, base$(&heap));

    IoWriter source_buf_writer = buf_writer(&source_buf);

    io_copy(&source_file_reader, &source_buf_writer);

    Scan scan;
    scan_init(&scan, buf_str(&source_buf));

    Lex lex = clex(&scan, base$(&heap));

    if (scan_dump_error(&scan, io_std_err()))
    {
        return -1;
    }

    CUnit unit = cparse_unit(&lex, base$(&heap));

    Buffer object_buffer;
    buffer_init(&object_buffer, 512, base$(&heap));

    IoWriter object_buffer_writer = buffer_writer(&source_buffer);

    codegen_from_unit(unit, &object_buffer_writer);

    Emit emit;
    emit_init(&emit, io_std_out());

    emit_fmt(&emit, "\n");

    emit_fmt(&emit, "--- BEGIN OG CODE ---\n");
    emit_fmt(&emit, "{}\n", buf_str(&source_buf));
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
    cgen_c_unit(&emit, unit);
    emit_fmt(&emit, "--- END CODE ---\n");
    emit_fmt(&emit, "\n");

    heap_alloc_deinit(&heap);

    return 0;
}
