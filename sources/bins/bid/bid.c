#include <bid/gen.h>
#include <bid/parse.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include <cc/gen.h>
#include <json/emit.h>

void bid_emit_json(BidIface const iface, IoWriter *writer)
{
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    Json json = bidgen_json_iface(iface, alloc_global());
    heap_alloc_deinit(&heap);

    Emit emit;
    emit_init(&emit, writer);
    json_emit(json, &emit);
    emit_deinit(&emit);

    heap_alloc_deinit(&heap);
}

void bid_emit_c(BidIface const iface, IoWriter *writer)
{
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    CUnit unit = bidgen_c(&iface, alloc_global());
    heap_alloc_deinit(&heap);

    Emit emit;
    emit_init(&emit, writer);
    cgen_c_unit(&emit, unit);
    emit_deinit(&emit);

    heap_alloc_deinit(&heap);
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        log$("usage: bid input option");
        log$("");
        log$("options:");
        log$("  --output-json   set the output to json");
        log$("  --output-c      set the output to c");

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

    BidIface iface = bid_parse(&scan, base$(&heap));

    if (scan_dump_error(&scan, io_std_err()))
    {
        return -1;
    }

    if (str_eq(str$("--output-json"), str$(argv[2])))
    {
        bid_emit_json(iface, io_std_out());
    }
    else if (str_eq(str$("--output-c"), str$(argv[2])))
    {
        bid_emit_c(iface, io_std_out());
    }
    else
    {
        panic$("Unknow option {}", argv[2]);
    }

    return 0;
}
