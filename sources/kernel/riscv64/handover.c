#include <brutal/base.h>
#include <brutal/debug.h>
#include <brutal/ds/range.h>
#include <hw/fdt/fdt.h>
#include <loader/protocol.h>
#include <stdint.h>
#include "kernel/kernel.h"

extern uintptr_t _kernel_end;
extern uintptr_t _kernel_start;

static void handover_mmap_remove_entry(Handover *self, HandoverMmapEntry entry)
{
    int id = -1;

    for (int i = 0; i < (int)self->mmap.size; i++)
    {
        if (range_eq(self->mmap.entries[i], entry) && entry.type == self->mmap.entries[i].type)
        {
            id = i;
            break;
        }
    }

    assert_not_equal(id, -1);

    for (size_t i = id; i < self->mmap.size - id - 1; i++)
    {
        self->mmap.entries[i] = self->mmap.entries[i + 1];
    }

    self->mmap.size--;
}

static void handover_mmap_push_entry(Handover *self, HandoverMmapEntry entry)
{
    if (self->mmap.size >= HANDOVER_MMAP_MAX_SIZE)
    {
        panic$("Handover: mmap table full");
    }

    if (entry.size == 0)
    {
        return;
    }

    // use a temporary copy for easier handling of removing / adding an entry in a for loop
    Handover res = *self;

    for (size_t i = 0; i < self->mmap.size; i++)
    {
        if (range_colide(self->mmap.entries[i], entry))
        {
            handover_mmap_remove_entry(&res, self->mmap.entries[i]);
            HandoverMmapEntry left_range = range_half_under(self->mmap.entries[i], entry);
            HandoverMmapEntry right_range = range_half_over(self->mmap.entries[i], entry);
            left_range.type = self->mmap.entries[i].type;
            right_range.type = self->mmap.entries[i].type;

            handover_mmap_push_entry(&res, left_range);
            handover_mmap_push_entry(&res, right_range);
        }
    }

    res.mmap.entries[res.mmap.size++] = entry;

    *self = res;
}

static void arch_init_handover_memory(Handover *self, FdtHeader *header)
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

    handover_mmap_push_entry(self, entry);
}
static Iter arch_load_reserved_memory_iter(FdtNode *val, Handover *ctx)
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

static void arch_load_reserved_memory(FdtHeader *header, Handover *handover)
{
    FdtNode reserved_memory_node = fdt_lookup_node(header, str$("/reserved-memory"));

    fdt_node_childs(reserved_memory_node, (IterFn *)arch_load_reserved_memory_iter, handover);
}

static void arch_align_handover_mmap(Handover *self)
{
    for (size_t i = 0; i < self->mmap.size; i++)
    {
        if (self->mmap.entries[i].type == HANDOVER_MMAP_FREE)
        {
            self->mmap.entries[i].base = ALIGN_UP(self->mmap.entries[i].base, MEM_PAGE_SIZE);
            self->mmap.entries[i].size = ALIGN_DOWN(self->mmap.entries[i].size, MEM_PAGE_SIZE);
        }
        else
        {
            self->mmap.entries[i].base = ALIGN_DOWN(self->mmap.entries[i].base, MEM_PAGE_SIZE);
            self->mmap.entries[i].size = ALIGN_UP(self->mmap.entries[i].size, MEM_PAGE_SIZE);
        }
    }
}

static void arch_init_handover_mmap(Handover *self, FdtHeader *header)
{
    arch_init_handover_memory(self, header);

    HandoverMmapEntry kernel_entry = {
        .base = (uintptr_t)&_kernel_start,
        .size = (uintptr_t)&_kernel_end - (uintptr_t)&_kernel_start,
        .type = HANDOVER_MMAP_KERNEL_MODULE,
    };

    handover_mmap_push_entry(self, kernel_entry);

    arch_load_reserved_memory(header, self);
    arch_align_handover_mmap(self);
}

Handover arch_create_handover(uint64_t fdt_addr)
{
    Handover handover = {};
    handover.fdt = fdt_addr;
    arch_init_handover_mmap(&handover, (FdtHeader *)fdt_addr);
    return handover;
}
