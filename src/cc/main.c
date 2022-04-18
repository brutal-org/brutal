#include <brutal-debug>
#include <brutal-fmt>
#include <cc/dump.h>
#include <cc/lex/pproc.h>
#include <cc/parse.h>
#include <cc/sema.h>
#include <cc/trans.h>
#include <json/emit.h>

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

    Lex unprocessed = clex(&scan, base$(&heap));
    Lex processed = cproc_file(&unprocessed, str$(argv[1]), base$(&heap));

    if (scan_dump_error(&scan, io_chan_err()))
    {
        return -1;
    }

    CUnit unit = cparse_unit(&processed, base$(&heap));
    CSema sema;
    csema_init(&sema, base$(&heap));
    unit = csema_unit(&sema, unit, base$(&heap));
    Emit emit;
    emit_init(&emit, io_chan_out());

    emit_fmt$(&emit, "\n");

    emit_fmt$(&emit, "--- BEGIN OG CODE ---\n");
    emit_fmt$(&emit, "{}\n", buf_str(&source_buf));
    emit_fmt$(&emit, "--- END OG CODE ---\n");

    emit_fmt$(&emit, "\n");
    emit_fmt$(&emit, "\n");

    emit_ident_size(&emit, 2);
    emit_fmt$(&emit, "--- BEGIN AST ---\n");
    Json json = cdump_unit(unit, base$(&heap));
    json_emit(json, &emit);
    emit_fmt$(&emit, "--- END AST ---\n");

    emit_fmt$(&emit, "\n");
    emit_fmt$(&emit, "\n");

    emit_ident_size(&emit, 4);

    emit_fmt$(&emit, "--- BEGIN CODE ---\n");
    cc_trans_unit(&emit, unit);
    emit_fmt$(&emit, "--- END CODE ---\n");
    emit_fmt$(&emit, "\n");

    heap_alloc_deinit(&heap);

    return 0;
}
