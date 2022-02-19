#include <brutal/debug.h>
#include <brutal/mem.h>
#include <loader/loader.h>
#include "loader/memory.h"

uint64_t kernel_module_phys_alloc_page(size_t count)
{
    uint64_t res = 0;

    loader_acquire_pages(count, &res, LOADER_MEM_KERNEL_PAGES);

    mem_set((void *)res, 0, PAGE_SIZE * count);

    return res;
}

uint64_t kernel_module_phys_alloc_page_addr(size_t count, uint64_t addr)
{
    uint64_t res = addr;

    loader_acquire_pages(count, &res, LOADER_MEM_ADDR);

    mem_set((void *)res, 0, PAGE_SIZE * count);

    return res;
}

uint64_t loader_phys_alloc_page(size_t count)
{
    uint64_t res = 0;

    loader_acquire_pages(count, &res, LOADER_MEM_DATA_PAGES);

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

void memory_switch(VmmSpace page_table)
{
    asm volatile("mov %0, %%cr3" ::"a"((uintptr_t)page_table));
}

void memory_map_range(VmmSpace self, VmmRange vmm_range, PmmRange pmm_range)
{
    for (size_t i = 0; i < m_min(vmm_range.size, pmm_range.size); i += PAGE_SIZE)
    {
        memory_map(i + pmm_range.base, i + vmm_range.base, self);
    }
}

VmmSpace memory_create(void)
{
    VmmSpace self = (Pages *)loader_phys_alloc_page(1);

    memory_map_range(self, (VmmRange){
                               .base = memory_phys_to_io(0),
                               .size = GiB(4),
                           },
                     (PmmRange){
                         .base = 0,
                         .size = GiB(4),
                     });

    memory_map_range(self, (VmmRange){
                               .base = (0),
                               .size = GiB(4),
                           },
                     (PmmRange){
                         .base = 0,
                         .size = GiB(4),
                     });

    return self;
}
