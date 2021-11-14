#include <brutal/debug.h>
#include <brutal/mem.h>
#include <efi/lib.h>
#include <efi/srvs/bs.h>
#include <embed/mem.h>
#include "loader/memory.h"

static Pages *pages;

uint64_t kernel_module_phys_alloc_page(size_t count, uint64_t addr)
{
    uint64_t res = addr;

    // type, memory type, pages, memory
    EFIStatus status = efi_st()->boot_services->allocate_pages(ALLOCATE_ADDRESS, UEFI_MEM_BRUTAL_KERNEL_MODULE, count, &res);

    if (status != EFI_SUCCESS)
    {
        log$("Failed to allocate {} pages of memory: {}", count, status);
        return 0;
    }

    mem_set((void *)res, 0, PAGE_SIZE * count);

    return res;
}

uint64_t loader_phys_alloc_page(size_t count)
{
    uint64_t res = 0;

    // type, memory type, pages, memory
    EFIStatus status = efi_st()->boot_services->allocate_pages(ALLOCATE_ANY_PAGES, EFI_BOOT_SERVICES_DATA, count, &res);

    if (status != EFI_SUCCESS)
    {
        log$("Failed to allocate {} pages of memory: {}", count, status);
        return 0;
    }

    mem_set((void *)res, 0, PAGE_SIZE * count);

    return res;
}

uint64_t get_page_or_map(Pages *dir, size_t idx)
{

    if (dir[idx] & PAGE_PRESENT)
    {
        return dir[idx] & PAGE_ADDR;
    }

    dir[idx] = loader_phys_alloc_page(1) | LOADER_PAGING_FLAGS;

    return dir[idx] & PAGE_ADDR;
}

void memory_map(uint64_t phys, uint64_t virt, Pages *pml4)
{
    Pages *pml3 = (Pages *)get_page_or_map(pml4, PML4_GET_INDEX(virt));
    Pages *pml2 = (Pages *)get_page_or_map(pml3, PML3_GET_INDEX(virt));
    Pages *pml1 = (Pages *)get_page_or_map(pml2, PML2_GET_INDEX(virt));

    size_t pml1_idx = PML1_GET_INDEX(virt);

    if (pml1[pml1_idx] & PAGE_PRESENT)
    {
        panic$("page: {} already mapped to: {}", virt, pml1[pml1_idx]);
    }

    pml1[pml1_idx] = phys | LOADER_PAGING_FLAGS;
}

void memory_map_range(VmmRange vmm_range, PmmRange pmm_range)
{
    for (size_t i = 0; i < MIN(vmm_range.size, pmm_range.size); i += PAGE_SIZE)
    {
        memory_map(i + pmm_range.base, i + vmm_range.base, pages);
    }
}

void memory_init(void)
{
    log$("allocated: {x}", sizeof(Pages));
    pages = (Pages *)loader_phys_alloc_page(1);

    log$("loading bootloader memory map");

    memory_map_range((VmmRange){
                         .base = memory_phys_to_io(0),
                         .size = GiB(4),
                     },
                     (PmmRange){
                         .base = 0,
                         .size = GiB(4),
                     });
    memory_map_range((VmmRange){
                         .base = (0),
                         .size = GiB(4),
                     },
                     (PmmRange){
                         .base = 0,
                         .size = GiB(4),
                     });

    asm volatile("mov %0, %%cr3" ::"a"((uintptr_t)pages));
}

void memory_flush_tlb(void)
{
    asm volatile("mov %0, %%cr3" ::"a"((uintptr_t)pages));
}
