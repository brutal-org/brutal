#include <brutal/debug.h>
#include <brutal/sync.h>
#include "kernel/heap.h"
#include <brutal/debug.h>
#include "kernel/mmap.h"
#include "kernel/vmm.h"
#include "kernel/heap.h"
#include "kernel/riscv64/asm.h"
#include <brutal/base.h>

#include "kernel/riscv64/paging.h"
#include "kernel/pmm.h"

#define _cpu_paging_level DEFAULT_PAGING_DEPTH

VmmResult vmm_flush(MAYBE_UNUSED VmmSpace space, VmmRange virtual_range)
{
    return OK(VmmResult, virtual_range);
}

int cpu_paging_level(void)
{
    /* FIXME: use a better aproach, and detect wich depth we are targetting */
    return _cpu_paging_level;
}

uint64_t satp_mode_from_paging_level(int level)
{
    switch(level)
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
            return 0;
    }
}

bool vmm_map_page(VmmSpace space, uintptr_t virtual, uintptr_t physical, BrMemoryFlags flags)
{
    SvPageTable* table = space;
    uintptr_t pvirt = virtual >> 12;

    for(int i = _cpu_paging_level - 1; i > 0; i--)
    {
        uintptr_t entry = SVTABLE_GET_INDEX(virtual, i);
        if(table->pages[entry].valid == 0)
        {
            uintptr_t next = mmap_io_to_phys(UNWRAP(heap_alloc(MEM_PAGE_SIZE)).base);
            log$("next: {#x} - {} ({#x})", next, i, virtual);
            table->pages[entry] = sv_make_entry(flags, next, false);
        }

        table = (SvPageTable*)mmap_phys_to_io((table->pages[entry].ppn) << 12);
    }

    table->pages[pvirt & 0x1ff] = sv_make_entry(flags, physical, true);
    return true;

}
void vmm_initialize(MAYBE_UNUSED Handover const *handover)
{
    SvPageTable* root = (SvPageTable*)UNWRAP(heap_alloc(MEM_PAGE_SIZE)).base;
    log$("{#x}",(uintptr_t) root);
    for(int i = 0; i < 512; i++)
    {
        root->pages[i]._raw = 0;
    }
    for(size_t i = 0x80000000 - 10; i < (0x88000000 + 10); i+= MEM_PAGE_SIZE)
    {
        vmm_map_page((root), align_up$( i, MEM_PAGE_SIZE), align_up$(i, MEM_PAGE_SIZE), BR_MEM_EXECUTABLE | BR_MEM_READABLE | BR_MEM_WRITABLE);
    }

    write_csr(CSR_SATP, satp_mode_from_paging_level(_cpu_paging_level) << 60 | ((uintptr_t)root >> 12));
    log$("paging in riscv ?");

}

VmmSpace vmm_space_create(void)
{
    return nullptr;
}

void vmm_space_destroy(MAYBE_UNUSED VmmSpace space)
{
}

void vmm_space_switch(MAYBE_UNUSED VmmSpace space)
{
}

VmmSpace vmm_kernel_space(void)
{
    return nullptr;
}

VmmResult vmm_map(MAYBE_UNUSED VmmSpace space, MAYBE_UNUSED VmmRange virtual_range, MAYBE_UNUSED PmmRange physical_range, MAYBE_UNUSED BrMemoryFlags flags)
{
    return OK(VmmResult, virtual_range);
}

VmmResult vmm_unmap(MAYBE_UNUSED VmmSpace space, VmmRange virtual_range)
{

    return OK(VmmResult, virtual_range);
}

PmmResult vmm_virt2phys(MAYBE_UNUSED VmmSpace space, MAYBE_UNUSED VmmRange virtual_range)
{

    return OK(PmmResult, (PmmRange){});
}
