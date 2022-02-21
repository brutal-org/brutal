#include <brutal/alloc.h>
#include <brutal/codec/ssfn2/ssfn2.h>
#include <brutal/debug.h>
#include <brutal/io.h>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        log$("Usage: fontinfo [OPTION]... [INPUT]", argv[0]);
        return -1;
    }

    IoFile source_file;
    UNWRAP_OR_PANIC(io_file_open(&source_file, str$(argv[1]), FILE_OPEN_READ_ONLY), "File not found!");

    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    SSFN2Collection ssfn_coll;
    if (!ssfn2_load(io_file_rseek(&source_file), &ssfn_coll, base$(&heap)).succ)
    {
        return -1;
    }

    log$("SSFN2 Font Collection: {} entries", ssfn_coll.len);

    size_t idx = 0;
    vec_foreach(font, &ssfn_coll)
    {
        log$("");
        log$("[{}] Name: {}", idx, font->stringtable.font_name);
        log$("[{}] Family: {}", idx, font->stringtable.family_name);
        log$("[{}] Subfamily: {}", idx, font->stringtable.subfamily_name);

        idx++;
    }

    heap_alloc_deinit(&heap);

    return 0;
}
