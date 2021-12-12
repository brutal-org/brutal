#include <bid/gen.h>
#include <bid/parse.h>
#include <bid/pass.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include <cc/trans.h>
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

void bid_emit_source(BidIface const iface, IoWriter *writer)
{
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    CUnit unit = bidgen_c_source(iface, alloc_global());
    heap_alloc_deinit(&heap);

    Emit emit;
    emit_init(&emit, writer);
    cgen_c_unit(&emit, unit);
    emit_deinit(&emit);

    heap_alloc_deinit(&heap);
}

void bid_emit_header(BidIface const iface, IoWriter *writer)
{
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    CUnit unit = bidgen_c_header(iface, alloc_global());
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
        log$("Usage: {} [file] [options...]", argv[0]);
        log$("");
        log$("Options:");
        log$("  --json   generate a json object");
        log$("  --header generate a C header file");
        log$("  --source generate a C source file");

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

    BidIface iface = bid_parse(&scan, base$(&heap));

    if (scan_dump_error(&scan, io_chan_err()))
    {
        return -1;
    }

    if (str_eq(str$("--json"), str$(argv[2])))
    {
        iface = bid_pass_prefix(iface, base$(&heap));
        bid_emit_json(iface, io_chan_out());
    }
    else if (str_eq(str$("--source"), str$(argv[2])))
    {
        iface = bid_pass_prefix(iface, base$(&heap));
        bid_emit_source(iface, io_chan_out());
    }
    else if (str_eq(str$("--header"), str$(argv[2])))
    {
        iface = bid_pass_prefix(iface, base$(&heap));
        bid_emit_header(iface, io_chan_out());
    }
    else
    {
        panic$("Unknow option {}", argv[2]);
    }

    return 0;
}
