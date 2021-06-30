#include <brutal/log.h>
#include <brutal/sync.h>
#include "arch/vmm.h"
#include "arch/x86_64/asm.h"
#include "arch/x86_64/memory/mmap.h"
#include "arch/x86_64/paging.h"

static struct pml *kernel_pml = nullptr;
static Lock vmm_lock;

static vmm_Result vmm_get_pml(struct pml *table, size_t idx)
{
    auto entry = table->entries[idx];

    if (entry.present)
    {
        VmmRange range = {
            mmap_phys_to_io(entry.physical << 12),
            HOST_MEM_PAGESIZE,
        };

        return OK(vmm_Result, range);
    }
    else
    {
        return ERR(vmm_Result, BR_ERR_BAD_ADDRESS);
    }
}

static vmm_Result vmm_get_pml_or_alloc(struct pml *table, size_t idx, size_t flags)
{
    auto entry = table->entries[idx];

    if (entry.present)
    {
        VmmRange range = {
            mmap_phys_to_io(entry.physical << 12),
            HOST_MEM_PAGESIZE,
        };

        return OK(vmm_Result, range);
    }
    else
    {
        uintptr_t target = TRY(vmm_Result, pmm_alloc(HOST_MEM_PAGESIZE)).base;

        mem_set((void *)mmap_phys_to_io(target), 0, HOST_MEM_PAGESIZE);
        table->entries[idx] = pml_make_entry(target, flags);

        VmmRange range = {mmap_phys_to_io(target), HOST_MEM_PAGESIZE};
        return OK(vmm_Result, range);
    }
}

static vmm_Result vmm_map_page(struct pml *pml4, uintptr_t virtual_page, uintptr_t physical_page, BrMemFlags flags)
{
    auto pml3_range = TRY(vmm_Result, vmm_get_pml_or_alloc(pml4, PML4_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER));
    struct pml *pml3 = (struct pml *)(pml3_range.base);

    auto pml2_range = TRY(vmm_Result, vmm_get_pml_or_alloc(pml3, PML3_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER));
    struct pml *pml2 = (struct pml *)(pml2_range.base);

    auto pml1_range = TRY(vmm_Result, vmm_get_pml_or_alloc(pml2, PML2_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER));
    struct pml *pml1 = (struct pml *)(pml1_range.base);

    pml1->entries[PML1_GET_INDEX(virtual_page)] = pml_make_entry(physical_page, flags);

    VmmRange range = {virtual_page, HOST_MEM_PAGESIZE};
    return OK(vmm_Result, range);
}

static vmm_Result vmm_unmap_page(struct pml *pml4, uintptr_t virtual_page)
{
    auto pml3_range = TRY(vmm_Result, vmm_get_pml(pml4, PML4_GET_INDEX(virtual_page)));
    struct pml *pml3 = (struct pml *)(pml3_range.base);

    auto pml2_range = TRY(vmm_Result, vmm_get_pml(pml3, PML3_GET_INDEX(virtual_page)));
    struct pml *pml2 = (struct pml *)(pml2_range.base);

    auto pml1_range = TRY(vmm_Result, vmm_get_pml(pml2, PML2_GET_INDEX(virtual_page)));
    struct pml *pml1 = (struct pml *)(pml1_range.base);

    pml1->entries[PML1_GET_INDEX(virtual_page)] = pml_clean_entry();

    VmmRange range = {virtual_page, HOST_MEM_PAGESIZE};
    return OK(vmm_Result, range);
}

// later we may use an other initializer that fork the higher kernel vmm entry
static void vmm_load_memory_map(vmm_space_t target, struct handover_mmap const *memory_map)
{
    log("Loading kernel memory map...");
    vmm_map(target,
            (VmmRange){
                .base = mmap_phys_to_io(0),
                .size = 0xffffffff},
            (PmmRange){
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
                    (VmmRange){
                        .base = mmap_phys_to_kernel(ALIGN_DOWN(entry.base, HOST_MEM_PAGESIZE)),
                        .size = ALIGN_UP(entry.length, HOST_MEM_PAGESIZE) + HOST_MEM_PAGESIZE},
                    (PmmRange){
                        .base = ALIGN_DOWN(entry.base, HOST_MEM_PAGESIZE),
                        .size = ALIGN_UP(entry.length, HOST_MEM_PAGESIZE) + HOST_MEM_PAGESIZE},
                    BR_MEM_WRITABLE);
        }

        vmm_map(target,
                (VmmRange){
                    .base = mmap_phys_to_io(entry.base),
                    .size = entry.length},
                (PmmRange){
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
    LOCK_RETAINER(&vmm_lock);

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
    LOCK_RETAINER(&vmm_lock);

    asm_write_cr3(mmap_io_to_phys((uintptr_t)space));
}

vmm_Result vmm_map(vmm_space_t space, VmmRange virtual_range, PmmRange physical_range, BrMemFlags flags)
{
    LOCK_RETAINER(&vmm_lock);

    if (virtual_range.size != physical_range.size)
    {
        panic("virtual_range.size must be equal to physical_range for the moment");
        return ERR(vmm_Result, -1);
    }

    for (size_t i = 0; i < (virtual_range.size / HOST_MEM_PAGESIZE); i++)
    {
        vmm_map_page(
            space,
            i * HOST_MEM_PAGESIZE + ALIGN_DOWN(virtual_range.base, HOST_MEM_PAGESIZE),
            i * HOST_MEM_PAGESIZE + ALIGN_DOWN(physical_range.base, HOST_MEM_PAGESIZE),
            flags);
    }

    return OK(vmm_Result, virtual_range);
}

vmm_Result vmm_unmap(vmm_space_t space, VmmRange virtual_range)
{
    for (size_t i = 0; i < (virtual_range.size / HOST_MEM_PAGESIZE); i++)
    {
        vmm_unmap_page(space, i * HOST_MEM_PAGESIZE + ALIGN_DOWN(virtual_range.base, HOST_MEM_PAGESIZE));
    }

    return OK(vmm_Result, virtual_range);
}

pmm_Result vmm_virt2phys(vmm_space_t space, VmmRange virtual_range)
{
    LOCK_RETAINER(&vmm_lock);

    struct pml *pml4 = space;

    auto pml3_range = TRY(pmm_Result, vmm_get_pml(pml4, PML4_GET_INDEX(virtual_range.base)));
    struct pml *pml3 = (struct pml *)(pml3_range.base);

    auto pml2_range = TRY(pmm_Result, vmm_get_pml(pml3, PML3_GET_INDEX(virtual_range.base)));
    struct pml *pml2 = (struct pml *)(pml2_range.base);

    auto pml1_range = TRY(pmm_Result, vmm_get_pml(pml2, PML2_GET_INDEX(virtual_range.base)));
    struct pml *pml1 = (struct pml *)(pml1_range.base);

    auto entry = pml1->entries[PML1_GET_INDEX(virtual_range.base)];
    PmmRange range = {entry.physical << 12, 4096};

    return OK(pmm_Result, range);
}

vmm_space_t vmm_kernel_space(void)
{
    return (vmm_space_t)kernel_pml;
}
