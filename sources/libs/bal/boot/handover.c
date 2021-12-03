#include <bal/boot/handover.h>
#include <brutal/debug.h>

HandoverModule const *handover_find_module(Handover const *handover, Str name)
{
    for (size_t i = 0; i < handover->modules.size; i++)
    {
        if (str_eq(name, str$(&handover->modules.module[i].module_name)))
        {
            return &handover->modules.module[i];
        }
    }

    return nullptr;
}

void handover_dump(Handover const *handover)
{
    if (handover->tag != HANDOVER_TAG)
    {
        log$("not valid handover tag: {}", handover->tag);
    }


    log$("Memory Map:");

    for (size_t i = 0; i < handover->mmap.size; i++)
    {
        HandoverMmapEntry entry = handover->mmap.entries[i];

        log$("\t{#p}-{#p} {}", entry.base, entry.base + entry.size - 1, ho_mmtype_to_str(entry.type));
    }

    log$("Framebuffer:");

    if (handover->framebuffer.present)
    {
        log$("\tAddress: {#p}", handover->framebuffer.addr);
        log$("\tWidth: {}", handover->framebuffer.width);
        log$("\tHeight: {}", handover->framebuffer.height);
        log$("\tPitch: {}", handover->framebuffer.pitch);
        log$("\tBpp: {}", handover->framebuffer.bpp);
    }
    else
    {
        log$("\tNot found!");
    }

    log$("Modules:");

    for (size_t i = 0; i < handover->modules.size; i++)
    {
        HandoverModule entry = handover->modules.module[i];

        log$("\t{#p}-{#p} {#}", entry.addr, entry.addr + entry.size, str$(&entry.module_name));
    }

    log$("Rsdp:");
    log$("\tAddress: {#p}", handover->rsdp);

    log$("Cmd lines:");

    if (handover->cmd_lines.present)
    {
        log$("\t value: '{}'", str$(&handover->cmd_lines.cmd_line));
    }
    else
    {
        log$("\tNot found!");
    }
}

void handover_mmap_remove_entry(HandoverMmap *self, HandoverMmapEntry entry)
{
    int id = -1;

    for (int i = 0; i < (int)self->size; i++)
    {
        if (range_eq(self->entries[i], entry) && entry.type == self->entries[i].type)
        {
            id = i;
            break;
        }
    }

    assert_not_equal(id, -1);

    for (size_t i = id; i < self->size - id - 1; i++)
    {
        self->entries[i] = self->entries[i + 1];
    }

    self->size--;
}

void handover_mmap_push_entry(HandoverMmap *self, HandoverMmapEntry entry)
{
    if (self->size >= HANDOVER_MMAP_MAX_SIZE)
    {
        panic$("Handover: mmap table full");
    }

    if (entry.size == 0)
    {
        return;
    }

    // use a temporary copy for easier handling of removing / adding an entry in a for loop
    HandoverMmap res = *self;

    for (size_t i = 0; i < self->size; i++)
    {
        if (range_colide(self->entries[i], entry))
        {
            handover_mmap_remove_entry(&res, self->entries[i]);
            HandoverMmapEntry left_range = range_half_under(self->entries[i], entry);
            HandoverMmapEntry right_range = range_half_over(self->entries[i], entry);
            left_range.type = self->entries[i].type;
            right_range.type = self->entries[i].type;

            handover_mmap_push_entry(&res, left_range);
            handover_mmap_push_entry(&res, right_range);
        }
    }

    res.entries[res.size++] = entry;

    *self = res;
}

static Iter handover_mmap_load_reserved_memory_iter(FdtNode *val, HandoverMmap *ctx)
{
    if (str_count(val->name, str$("mmode_resv")) == -1)
    {
        return ITER_CONTINUE;
    }

    FdtProp memory_prop = fdt_lookup_props(*val, str$("reg"));

    // FIXME: we assume #address-cells and #size-cells is equal to 2
    USizeRange resv_mem = fdt_reg_get_range(memory_prop, 2, 2);

    handover_mmap_push_entry(ctx,
                             (HandoverMmapEntry){
                                 .base = resv_mem.base,
                                 .size = resv_mem.size,
                                 .type = HANDOVER_MMAP_RESERVED,
                             });

    return ITER_CONTINUE;
}

static void arch_load_reserved_memory(FdtHeader *header, HandoverMmap *handover)
{
    FdtNode reserved_memory_node = fdt_lookup_node(header, str$("/reserved-memory"));

    fdt_node_childs(reserved_memory_node, (IterFn *)handover_mmap_load_reserved_memory_iter, handover);
}

static void init_handover_memory(Handover *self, FdtHeader *header)
{
    FdtNode root_node = fdt_lookup_node(header, str$(""));
    FdtProp root_addr_size = fdt_lookup_props(root_node, str$("#address-cells"));
    FdtProp root_len_size = fdt_lookup_props(root_node, str$("#size-cells"));

    // TODO: for the moment we don't support 32bit address size
    int addr_size = load_be(*(be_uint32_t *)root_addr_size.value.buf);
    int len_size = load_be(*(be_uint32_t *)root_len_size.value.buf);

    // getting /memory@*/reg
    FdtProp memory_reg_prop = fdt_lookup_props(fdt_lookup_node(header, str$("/memory@")), str$("reg"));

    USizeRange fdt_mem_range = fdt_reg_get_range(memory_reg_prop, addr_size, len_size);
    HandoverMmapEntry entry = {.base = fdt_mem_range.base, .size = fdt_mem_range.size, .type = HANDOVER_MMAP_FREE};

    handover_mmap_push_entry(&self->mmap, entry);
}

void init_handover_mmap(Handover *self, FdtHeader *header)
{
    init_handover_memory(self, header);

    arch_load_reserved_memory(header, &self->mmap);
    handover_mmap_sanitize(&self->mmap);
}

void handover_mmap_sanitize(HandoverMmap *self)
{
    HandoverMmap res = {};

    while (self->size > 0)
    {
        HandoverMmapEntry entry = {.base = (uintptr_t)-1};
        for (size_t i = 0; i < self->size; i++)
        {
            if (self->entries[i].base < entry.base)
            {
                entry = self->entries[i];
            }
        }

        handover_mmap_remove_entry(self, entry);
        handover_mmap_push_entry(&res, entry);
    }

    *self = res;
    // align
    for (size_t i = 0; i < self->size; i++)
    {
        if (self->entries[i].type == HANDOVER_MMAP_FREE)
        {
            self->entries[i].base = ALIGN_UP(self->entries[i].base, MEM_PAGE_SIZE);
            self->entries[i].size = ALIGN_DOWN(self->entries[i].size, MEM_PAGE_SIZE);
        }
        else
        {
            self->entries[i].base = ALIGN_DOWN(self->entries[i].base, MEM_PAGE_SIZE);
            self->entries[i].size = ALIGN_UP(self->entries[i].size, MEM_PAGE_SIZE);
        }
    }
}

void handover_init_from_fdt(Handover *target, FdtHeader *header)
{
    target->fdt = (uintptr_t)header;
    init_handover_mmap(target, header);
}
