#pragma once

#include <brutal/base.h>
#include <stdint.h>

typedef uintptr_t Pages;

typedef Range(uint64_t) VmmRange;
typedef Range(uint64_t) PmmRange;

#define UEFI_MEM_BRUTAL_KERNEL_MODULE (0x80000ff0)

#define PAGE_SIZE (0x1000)

#define PAGE_ADDR (~(((uint64_t)0b11111111111)))

#define PAGE_PRESENT (1)
// present | read write | user
#define LOADER_PAGING_FLAGS (PAGE_PRESENT | (1 << 1) | (1 << 2))

#define PMLX_GET_INDEX(ADDR, LEVEL) (((uint64_t)ADDR & ((uint64_t)0x1ff << (12 + LEVEL * 9))) >> (12 + LEVEL * 9))

#define PML4_GET_INDEX(ADDR) PMLX_GET_INDEX(ADDR, 3)
#define PML3_GET_INDEX(ADDR) PMLX_GET_INDEX(ADDR, 2)
#define PML2_GET_INDEX(ADDR) PMLX_GET_INDEX(ADDR, 1)
#define PML1_GET_INDEX(ADDR) PMLX_GET_INDEX(ADDR, 0)

#define MMAP_IO_BASE (0xffff800000000000)
#define MMAP_KERNEL_BASE (0xffffffff80000000)

void memory_init(void);

void memory_flush_tlb(void);

void memory_map_range(VmmRange vmm_range, PmmRange pmm_range);

uint64_t loader_phys_alloc_page(size_t count);

uint64_t kernel_module_phys_alloc_page_addr(size_t count, uint64_t addr);

uint64_t kernel_module_phys_alloc_page(size_t count);

static inline uintptr_t memory_phys_to_io(uintptr_t phys_addr)
{
    return phys_addr + MMAP_IO_BASE;
}
