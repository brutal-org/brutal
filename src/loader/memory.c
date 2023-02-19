#include <brutal-debug>
#include <brutal-mem>
#include <efi/lib.h>
#include <efi/srvs/bs.h>
#include <embed/mem.h>

#include "loader/memory.h"

uint64_t kernel_module_phys_alloc_page(size_t count)
{
    uint64_t res = 0;

    // type, memory type, pages, memory
    EfiStatus status = efi_st()->boot_services->allocate_pages(ALLOCATE_ANY_PAGES, EFI_USER_KERNEL_MEMORY, count, &res);

    if (status != EFI_SUCCESS)
    {
        log$("Failed to allocate {} pages of memory: {}", count, status);
        return 0;
    }

    mem_set((void *)res, 0, PAGE_SIZE * count);

    return res;
}

uint64_t kernel_module_phys_alloc_page_addr(size_t count, uint64_t addr)
{
    uint64_t res = addr;

    // type, memory type, pages, memory
    EfiStatus status = efi_st()->boot_services->allocate_pages(ALLOCATE_ADDRESS, EFI_USER_KERNEL_MEMORY, count, &res);

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
    EfiStatus status = efi_st()->boot_services->allocate_pages(ALLOCATE_ANY_PAGES, EFI_BOOT_SERVICES_DATA, count, &res);

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
        panic$("Page: {} already mapped to: {}", virt, pml1[pml1_idx]);
    }

    pml1[pml1_idx] = phys | LOADER_PAGING_FLAGS;
}

void memory_switch(VmmSpace space)
{
    asm volatile("mov %0, %%cr3"
                 :
                 : "a"((uintptr_t)space));
}

void memory_map_range(VmmSpace space, VmmRange vmm_range, PmmRange pmm_range)
{
    for (size_t i = 0; i < m_min(vmm_range.size, pmm_range.size); i += PAGE_SIZE)
    {
        memory_map(i + pmm_range.base, i + vmm_range.base, space);
    }
}

VmmSpace memory_create(void)
{
    VmmSpace self = (Pages *)loader_phys_alloc_page(1);

    size_t desc_count = 0;
    size_t desc_size = 0;
    EFIMemoryDescriptor *desc = efi_mmap_snapshot(&desc_count, &desc_size);

    for (size_t i = 0; i < desc_count; i++)
    {
        desc = (EFIMemoryDescriptor *)((void *)desc + desc_size);

        uint64_t base = desc->physical_start;
        uint64_t size = desc->num_pages << 12;

        memory_map_range(self,
                         (VmmRange){
                             .base = base,
                             .size = size,
                         },
                         (PmmRange){
                             .base = base,
                             .size = size,
                         });

        memory_map_range(self,
                         (VmmRange){
                             .base = base + MMAP_IO_BASE,
                             .size = size,
                         },
                         (PmmRange){
                             .base = base,
                             .size = size,
                         });
    }

    return self;
}
