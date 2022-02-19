#include <bal/abi.h>
#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include "loader/memory.h"
#include "loader/protocol.h"
#include <embed/handover.h>

static void load_module(HandoverModule *target, Str path)
{
    IoFile file;
    io_file_open(&file, path);

    // TODO: make a kernel_module alloc instead of allocating 2 time (even if the first time is always freed)
    Buf buf;
    io_read_all(io_file_reader(&file), &buf, alloc_global());

    uintptr_t buf_page_size = align_up$(buf.used, PAGE_SIZE) / PAGE_SIZE;
    uintptr_t module_addr = kernel_module_phys_alloc_page(buf_page_size);

    memcpy((void *)module_addr, buf.data, buf.used);

    target->size = align_up$(buf.used, PAGE_SIZE);
    target->addr = module_addr;

    buf_deinit(&buf);

    log$("Loaded module data '{}' (size: {})...", path, target->size);
}

static void load_modules(LoaderEntry const *entry, HandoverModules *modules)
{
    int id = 0;
    vec_foreach_v(module, &entry->modules)
    {
        HandoverModule *target = &modules->module[id];

        memcpy(target->module_name.buf, module.name.buf, module.name.len);
        target->module_name.len = module.name.len;

        load_module(target, module.path);

        id++;
    }

    modules->size = id;
}

void loader_populate_handover(LoaderEntry const *entry, Handover *ho)
{
    ho->tag = HANDOVER_TAG;

    embed_handover_fill_framebuffer(&ho->framebuffer, entry->framebuffer.width, entry->framebuffer.height);

    load_modules(entry, &ho->modules);

    embed_handover_fill_mmap(&ho->mmap);

    ho->rsdp = embed_handover_get_rsdp();
}
