#include <library/log.h>
#include "arch/vmm.h"
#include "arch/x86_64/asm.h"
#include "arch/x86_64/memory/mmap.h"
#include "arch/x86_64/paging.h"

static struct pml *kernel_pml = NULL;

static vmm_result_t vmm_get_pml(struct pml *table, size_t idx)
{
    auto entry = table->entries[idx];

    if (entry.present)
    {
        vmm_range_t range = {
            mmap_phys_to_io(entry.physical << 12),
            HOST_MEM_PAGESIZE,
        };

        return OK(vmm_result_t, range);
    }
    else
    {
        return ERR(vmm_result_t, BR_ERR_BAD_ADDRESS);
    }
}

vmm_space_t vmm_kernel_space(void)
{
    return (vmm_space_t)kernel_pml;
}

static vmm_result_t vmm_get_pml_or_alloc(struct pml *table, size_t idx, size_t flags)
{
    auto entry = table->entries[idx];

    if (entry.present)
    {
        vmm_range_t range = {
            mmap_phys_to_io(entry.physical << 12),
            HOST_MEM_PAGESIZE,
        };

        return OK(vmm_result_t, range);
    }
    else
    {
        uintptr_t target = TRY(vmm_result_t, pmm_alloc(HOST_MEM_PAGESIZE)).base;

        mem_set((void *)mmap_phys_to_io(target), 0, HOST_MEM_PAGESIZE);
        table->entries[idx] = pml_make_entry(target, flags);

        vmm_range_t range = {mmap_phys_to_io(target), HOST_MEM_PAGESIZE};
        return OK(vmm_result_t, range);
    }
}

static vmm_result_t vmm_map_pml_make_entry(struct pml *pml4, uintptr_t virtual_page, uintptr_t physical_page, br_mem_flags_t flags)
{
    auto pml3_range = TRY(vmm_result_t, vmm_get_pml_or_alloc(pml4, PML4_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER));
    struct pml *pml3 = (struct pml *)(pml3_range.base);

    auto pml2_range = TRY(vmm_result_t, vmm_get_pml_or_alloc(pml3, PML3_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER));
    struct pml *pml2 = (struct pml *)(pml2_range.base);

    auto pml1_range = TRY(vmm_result_t, vmm_get_pml_or_alloc(pml2, PML2_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER));
    struct pml *pml1 = (struct pml *)(pml1_range.base);

    pml1->entries[PML1_GET_INDEX(virtual_page)] = pml_make_entry(physical_page, flags);

    vmm_range_t range = {virtual_page, HOST_MEM_PAGESIZE};
    return OK(vmm_result_t, range);
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
    auto pmm_result = pmm_alloc(HOST_MEM_PAGESIZE);

    kernel_pml = (struct pml *)mmap_phys_to_io(UNWRAP(pmm_result).base);
    mem_set(kernel_pml, 0, HOST_MEM_PAGESIZE);

    vmm_load_memory_map(kernel_pml, &handover->mmap);
    vmm_space_switch(kernel_pml);

    log("Loaded kernel memory map!");
}

vmm_space_t vmm_space_create(void)
{
    auto pmm_result = pmm_alloc(HOST_MEM_PAGESIZE);

    vmm_space_t vmm_address_space = (vmm_space_t)mmap_phys_to_io(UNWRAP(pmm_result).base);
    mem_set(vmm_address_space, 0, HOST_MEM_PAGESIZE);

    struct pml *pml_table = (struct pml *)vmm_address_space;

    for (size_t i = 0; i < 255; i++)
    {
        pml_table->entries[i]._raw = 0;
    }

    for (size_t i = 255; i < 512; i++)
    {
        pml_table->entries[i] = kernel_pml->entries[i];
    }

    return vmm_address_space;
}

void vmm_space_switch(vmm_space_t space)
{
    asm_write_cr3(mmap_io_to_phys((uintptr_t)space));
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
        vmm_map_pml_make_entry(space,
                               i * HOST_MEM_PAGESIZE + ALIGN_DOWN(virtual_range.base, HOST_MEM_PAGESIZE),
                               i * HOST_MEM_PAGESIZE + ALIGN_DOWN(physical_range.base, HOST_MEM_PAGESIZE),
                               flags);
    }

    return OK(vmm_result_t, virtual_range);
}

pmm_result_t vmm_virt2phys(vmm_space_t space, vmm_range_t virtual_range)
{
    struct pml *pml4 = space;

    auto pml3_range = TRY(pmm_result_t, vmm_get_pml(pml4, PML4_GET_INDEX(virtual_range.base)));
    struct pml *pml3 = (struct pml *)(pml3_range.base);

    auto pml2_range = TRY(pmm_result_t, vmm_get_pml(pml3, PML3_GET_INDEX(virtual_range.base)));
    struct pml *pml2 = (struct pml *)(pml2_range.base);

    auto pml1_range = TRY(pmm_result_t, vmm_get_pml(pml2, PML2_GET_INDEX(virtual_range.base)));
    struct pml *pml1 = (struct pml *)(pml1_range.base);

    auto entry = pml1->entries[PML1_GET_INDEX(virtual_range.base)];
    pmm_range_t range = {entry.physical << 12, 4096};

    return OK(pmm_result_t, range);
}