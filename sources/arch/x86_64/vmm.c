#include <library/log.h>

#include "arch/vmm.h"
#include "arch/x86_64/mmap.h"
#include "arch/x86_64/paging.h"

typedef result_t(br_error_t, uintptr_t) page_or_alloc_result_t;

static struct pml *kernel_memory_map = NULL;

static page_or_alloc_result_t vmm_get_pml_or_alloc(struct pml *table, size_t idx, size_t flags)
{
    if (table[idx].present)
    {
        return OK(page_or_alloc_result_t, mmap_phys_to_io(table[idx].physical << 12));
    }
    else
    {
        uintptr_t target = (uintptr_t)(pmm_alloc(HOST_MEM_PAGESIZE)).ok.base;

        mem_set((void *)mmap_phys_to_io(target), 0, HOST_MEM_PAGESIZE);
        table[idx] = page(target, flags);

        return OK(page_or_alloc_result_t, mmap_phys_to_io(target));
    }

    return ERR(page_or_alloc_result_t, 0);
}

static size_t vmm_map_page(vmm_space_t space, uintptr_t virtual_page, uintptr_t physical_page, br_mem_flags_t flags)
{

    struct pml *pml3 = (struct pml *)(vmm_get_pml_or_alloc((struct pml *)space, PML4_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER).ok);

    if (pml3 == NULL)
    {
        panic("vmm_get_pml_or_alloc return 0 (for pml3) ");
    }

    struct pml *pml2 = (struct pml *)(vmm_get_pml_or_alloc(pml3, PMLN_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER).ok);

    if (pml2 == NULL)
    {
        panic("vmm_get_pml_or_alloc return 0 (for pml2) ");
    }

    struct pml *pml1 = (struct pml *)(vmm_get_pml_or_alloc(pml2, PAGE_DIR_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER).ok);

    if (pml1 == NULL)
    {
        panic("vmm_get_pml_or_alloc return 0 (for pml1) ");
    }

    pml1[PAGE_TABLE_GET_INDEX(virtual_page)] = page(physical_page, flags);

    return 0;
}

// later we may use an other initializer that fork the higher kernel vmm entry
static void vmm_load_memory_map(vmm_space_t target, struct handover_mmap const *memory_map)
{
    log("Loading kernel memory map...");
    vmm_map(target,
            (vmm_range_t){
                .base = mmap_phys_to_io(0),
                .size = 0xffffffff},
            (pmm_range_t){
                .base = 0,
                .size = 0xffffffff},
            BR_MEM_WRITABLE);

    for (size_t i = 0; i < memory_map->size; i++)
    {
        auto entry = memory_map->entries[i];

        log("Loading kernel memory map {}/{} ({x} - {x})",
            i,
            memory_map->size,
            entry.base,
            entry.base + entry.length);

        if (entry.type == HANDOVER_MMAP_KERNEL_MODULE)
        {
            vmm_map(target,
                    (vmm_range_t){
                        .base = mmap_phys_to_kernel(ALIGN_DOWN(entry.base, HOST_MEM_PAGESIZE)),
                        .size = ALIGN_UP(entry.length, HOST_MEM_PAGESIZE) + HOST_MEM_PAGESIZE},
                    (pmm_range_t){
                        .base = ALIGN_DOWN(entry.base, HOST_MEM_PAGESIZE),
                        .size = ALIGN_UP(entry.length, HOST_MEM_PAGESIZE) + HOST_MEM_PAGESIZE},
                    BR_MEM_WRITABLE);
        }

        vmm_map(target,
                (vmm_range_t){
                    .base = mmap_phys_to_io(entry.base),
                    .size = entry.length},
                (pmm_range_t){
                    .base = (entry.base),
                    .size = entry.length},
                BR_MEM_WRITABLE);
    }

    vmm_space_switch(target);
}

void vmm_initialize(struct handover const *handover)
{
    kernel_memory_map = (struct pml *)mmap_phys_to_io((pmm_alloc(HOST_MEM_PAGESIZE).ok).base);
    mem_set(kernel_memory_map, 0, HOST_MEM_PAGESIZE);

    vmm_load_memory_map(kernel_memory_map, &handover->mmap);
    vmm_space_switch(kernel_memory_map);

    log("Loaded kernel memory map!");
}

vmm_space_t vmm_space_create(void)
{
    vmm_space_t vmm_address_space = (vmm_space_t)mmap_phys_to_io((pmm_alloc(HOST_MEM_PAGESIZE).ok).base);
    mem_set(vmm_address_space, 0, HOST_MEM_PAGESIZE);
    struct pml *pml_table = (struct pml *)vmm_address_space;

    for (size_t i = 0; i < 255; i++)
    {
        pml_table[i]._raw = 0;
    }

    for (size_t i = 255; i < 512; i++)
    {
        pml_table[i] = ((struct pml *)kernel_memory_map)[i];
    }

    return vmm_address_space;
}

void vmm_space_switch(vmm_space_t space)
{
    asm volatile("mov %0, %%cr3" ::"a"(mmap_io_to_phys((uintptr_t)space)));
}

vmm_result_t vmm_map(vmm_space_t space, vmm_range_t virtual_range, pmm_range_t physical_range, br_mem_flags_t flags)
{
    if (virtual_range.size != physical_range.size)
    {
        panic("virtual_range.size must be equal to physical_range for the moment");
        return ERR(vmm_result_t, -1);
    }

    for (size_t i = 0; i < (virtual_range.size / HOST_MEM_PAGESIZE); i++)
    {
        vmm_map_page(space,
                     i * HOST_MEM_PAGESIZE + ALIGN_DOWN(virtual_range.base, HOST_MEM_PAGESIZE),
                     i * HOST_MEM_PAGESIZE + ALIGN_DOWN(physical_range.base, HOST_MEM_PAGESIZE),
                     flags);
    }

    return OK(vmm_result_t, virtual_range);
}
