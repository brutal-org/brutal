#include "kernel/vmm.h"
#include <brutal-debug>
#include <brutal-sync>
#include "kernel/heap.h"
#include "kernel/mmap.h"
#include "kernel/riscv64/asm.h"
#include "kernel/riscv64/paging.h"

static int _levels = DEFAULT_PAGING_DEPTH;
static Lock _lock = {};

VmmResult vmm_flush(MAYBE_UNUSED VmmSpace space, MAYBE_UNUSED VmmRange virtual_range)
{
    panic$("vmm_flush not implemented");
}

uint64_t satp_mode_from_paging_level(int level)
{
    switch (level)
    {
    case PAGING_SV32:
        return 1;

    case PAGING_SV39:
        return 8;

    case PAGING_SV48:
        return 9;

    case PAGING_SV57:
        return 10;

    default:
        panic$("level not supported: {}", level);
    }
}

bool vmm_map_page(VmmSpace space, uintptr_t virtual, uintptr_t physical, BrMemoryFlags flags)
{
    SvPageTable *table = space;
    uintptr_t pvirt = virtual >> 12;

    for (int i = _levels - 1; i > 0; i--)
    {
        uintptr_t entry = SVTABLE_GET_INDEX(virtual, i);
        if (table->pages[entry].valid == 0)
        {
            uintptr_t next = mmap_io_to_phys(UNWRAP(heap_alloc(MEM_PAGE_SIZE)).base);
            table->pages[entry] = sv_make_entry(flags, next, false);
        }

        table = (SvPageTable *)mmap_phys_to_io((table->pages[entry].ppn) << 12);
    }

    table->pages[pvirt & 0x1ff] = sv_make_entry(flags, physical, true);
    return true;
}

void vmm_initialize(MAYBE_UNUSED Handover const *handover)
{
    SvPageTable *root = (SvPageTable *)UNWRAP(heap_alloc(MEM_PAGE_SIZE)).base;
    mem_set(root, 0, MEM_PAGE_SIZE);

    for (size_t i = handover_mmap_base(&handover->mmap); i < handover_mmap_end(&handover->mmap); i += MEM_PAGE_SIZE)
    {
        vmm_map_page((root), align_up$(i, MEM_PAGE_SIZE), align_up$(i, MEM_PAGE_SIZE), BR_MEM_EXECUTABLE | BR_MEM_READABLE | BR_MEM_WRITABLE);
    }

    vmm_space_switch(root);

    log$("Loaded kernel memory map!");
}

VmmSpace vmm_space_create(void)
{
    panic$("vmm_space_create not implemented");
}

void vmm_space_destroy(MAYBE_UNUSED VmmSpace space)
{
    panic$("vmm_space_destroy not implemented");
}

void vmm_space_switch(MAYBE_UNUSED VmmSpace space)
{
    write_csr(CSR_SATP, satp_mode_from_paging_level(_levels) << 60 | ((uintptr_t)space >> 12));
}

VmmSpace vmm_kernel_space(void)
{
    panic$("vmm_kernel_space not implemented");
}

VmmResult vmm_map(MAYBE_UNUSED VmmSpace space, MAYBE_UNUSED VmmRange virtual_range, MAYBE_UNUSED PmmRange physical_range, MAYBE_UNUSED BrMemoryFlags flags)
{
    assert_truth(mem_is_range_page_aligned(virtual_range));
    assert_truth(mem_is_range_page_aligned(physical_range));

    LOCK_RETAINER(&_lock);

    if (virtual_range.size != physical_range.size)
    {
        panic$("virtual_range.size must be equal to physical_range.size");
        return ERR(VmmResult, BR_BAD_ARGUMENTS);
    }

    for (size_t i = 0; i < (virtual_range.size / MEM_PAGE_SIZE); i++)
    {
        vmm_map_page(
            space,
            i * MEM_PAGE_SIZE + virtual_range.base,
            i * MEM_PAGE_SIZE + physical_range.base,
            flags);
    }

    return OK(VmmResult, virtual_range);
}

VmmResult vmm_unmap(MAYBE_UNUSED VmmSpace space, MAYBE_UNUSED VmmRange virtual_range)
{
    panic$("vmm_unmap not implemented");
}

PmmResult vmm_virt2phys(MAYBE_UNUSED VmmSpace space, MAYBE_UNUSED VmmRange virtual_range)
{
    panic$("vmm_virt2phys not implemented");
}
