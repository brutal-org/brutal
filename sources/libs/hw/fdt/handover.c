#include <fdt/handover.h>

static void fdt_populate_mmap_usable(FdtHeader *header, Handover *self)
{
    FdtNode root_node = fdt_lookup_node(header, str$(""));
    FdtProp root_addr_size = fdt_lookup_props(root_node, str$("#address-cells"));
    FdtProp root_len_size = fdt_lookup_props(root_node, str$("#size-cells"));

    // TODO: for the moment we only support 32bit address size
    int addr_size = be_load$(*(be_uint32_t *)root_addr_size.value.buf);
    int len_size = be_load$(*(be_uint32_t *)root_len_size.value.buf);

    // getting /memory@*/reg
    FdtProp memory_reg_prop = fdt_lookup_props(fdt_lookup_node(header, str$("/memory@")), str$("reg"));

    USizeRange fdt_mem_range = fdt_reg_get_range(memory_reg_prop, addr_size, len_size);
    HandoverMmapEntry entry = {.base = fdt_mem_range.base, .size = fdt_mem_range.size, .type = HANDOVER_MMAP_FREE};

    handover_mmap_append(&self->mmap, entry);
}

static Iter iter_fdt(FdtNode *val, HandoverMmap *ctx)
{
    if (str_count(val->name, str$("mmode_resv")) == -1)
    {
        return ITER_CONTINUE;
    }

    FdtProp memory_prop = fdt_lookup_props(*val, str$("reg"));

    // FIXME: we assume #address-cells and #size-cells is equal to 2
    USizeRange resv_mem = fdt_reg_get_range(memory_prop, 2, 2);

    handover_mmap_append(
        ctx,
        (HandoverMmapEntry){
            .base = resv_mem.base,
            .size = resv_mem.size,
            .type = HANDOVER_MMAP_RESERVED,
        });

    return ITER_CONTINUE;
}

static void fdt_populate_mmap_reserved(FdtHeader *header, HandoverMmap *handover)
{
    FdtNode reserved_memory_node = fdt_lookup_node(header, str$("/reserved-memory"));

    fdt_node_childs(reserved_memory_node, (IterFn *)iter_fdt, handover);
}

void fdt_populate_handover(FdtHeader *fdt, Handover *ho)
{
    ho->tag = HANDOVER_TAG;
    ho->fdt = (uintptr_t)fdt;

    fdt_populate_mmap_usable(fdt, ho);
    fdt_populate_mmap_reserved(fdt, &ho->mmap);
}
