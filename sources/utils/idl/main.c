#include <cc/trans.h>
#include <idl/cgen.h>
#include <idl/json.h>
#include <idl/parse.h>
#include <idl/sema.h>
#include <json/emit.h>
#include <brutal-alloc>
#include <brutal-debug>
#include <brutal-io>

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
    UNWRAP_OR_PANIC(io_file_view(&source_file, str$(argv[1])), "File not found!");

    Buf source_buf;
    buf_init(&source_buf, 512, base$(&heap));

    io_copy(io_file_reader(&source_file), buf_writer(&source_buf));

    Scan scan;
    scan_init(&scan, buf_str(&source_buf));

    IdlModule module = idl_parse_module(&scan, base$(&heap));

    if (scan_dump_error(&scan, io_chan_err()))
    {
        return -1;
    }

    Emit emit;
    emit_init(&emit, io_chan_out());

    module = idl_sema_module(module, base$(&heap));

    if (str_eq(str$("--json"), str$(argv[2])))
    {
        Json json = idl_jgen_module(module, base$(&heap));
        json_emit(json, &emit);
    }
    else if (str_eq(str$("--source"), str$(argv[2])))
    {
        CUnit unit = idl_cgen_source(module, base$(&heap));
        cc_trans_unit(&emit, unit);
    }
    else if (str_eq(str$("--header"), str$(argv[2])))
    {
        CUnit unit = idl_cgen_header(module, base$(&heap));
        cc_trans_unit(&emit, unit);
    }
    else
    {
        panic$("Unknow option {}", argv[2]);
    }

    emit_deinit(&emit);

    return 0;
}
