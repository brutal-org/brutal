#include "arch/vmm.h"
#include "arch/x86_64/mmap.h"
#include "arch/x86_64/paging.h"
#include <library/log.h>
#include <library/mem.h>

#define PML4_GET_INDEX(addr) (((uint64_t)addr & ((uint64_t)0x1ff << 39)) >> 39)
#define PDPT_GET_INDEX(addr) (((uint64_t)addr & ((uint64_t)0x1ff << 30)) >> 30)
#define PAGE_DIR_GET_INDEX(addr) (((uint64_t)addr & ((uint64_t)0x1ff << 21)) >> 21)
#define PAGE_TABLE_GET_INDEX(addr) (((uint64_t)addr & ((uint64_t)0x1ff << 12)) >> 12)

static vmm_space_t kernel_memory_map = NULL;

struct PACKED pml
{
    union
    {
        uint64_t _raw;

        struct PACKED
        {
            bool present : 1;
            bool read_write : 1;
            bool user : 1;
            bool caching : 1;
            bool caching_disable : 1;
            bool accessed : 1;
            bool dirty : 1;
            bool huge_page : 1;
            bool global_page : 1;
            uint8_t _available : 3;
            uint64_t physical : 52;
        };
    };
};

static struct pml page(uintptr_t physical, size_t flags) // by default create a present page
{
    return (struct pml){
        .physical = physical >> 12,
        .user = (flags & BR_MEM_USER) != 0,
        .read_write = (flags & BR_MEM_WRITABLE) != 0,
        .present = true,
        .caching = false,
        .caching_disable = false,
        .accessed = false,
        .dirty = false,
        .huge_page = false,
        .global_page = false,
        ._available = 0};
}

_Static_assert(sizeof(struct pml) == sizeof(uint64_t), "pml must be 64 bit");

typedef result_t(br_error_t, uintptr_t) page_or_alloc_result_t;

static page_or_alloc_result_t get_page_or_alloc(struct pml *table, size_t idx, size_t flags)
{

    //log("VMM: get_page_or_alloc({x}, {x}, {x})", (uintptr_t)pml, (uintptr_t)idx, flags);
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

    struct pml *pdpt = (struct pml *)(get_page_or_alloc((struct pml *)space, PML4_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER).ok);

    if (pdpt == NULL)
    {
        panic("VMM: vmm_map_page get_page_or_alloc return 0 (for pdpt) ");
    }

    struct pml *pd = (struct pml *)(get_page_or_alloc(pdpt, PDPT_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER).ok);

    if (pd == NULL)
    {
        panic("VMM: vmm_map_page get_page_or_alloc return 0 (for pdpt) ");
    }

    struct pml *pt = (struct pml *)(get_page_or_alloc(pd, PAGE_DIR_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER).ok);

    if (pt == NULL)
    {
        panic("VMM: vmm_map_page get_page_or_alloc return 0 (for pdpt) ");
    }

    pt[PAGE_TABLE_GET_INDEX(virtual_page)] = page(physical_page, flags);

    return 0;
}

vmm_result_t vmm_map(vmm_space_t space, vmm_range_t virtual_range, pmm_range_t physical_range, br_mem_flags_t flags)
{
    if (virtual_range.size != physical_range.size)
    {

        panic("VMM: virtual_range.size must be equal to physical_range for the moment");
        return ERR(vmm_result_t, -1);
    }

    for (size_t i = 0; i < (virtual_range.size / HOST_MEM_PAGESIZE); i++)
    {
        vmm_map_page(space, i * HOST_MEM_PAGESIZE + ALIGN_DOWN(virtual_range.base, HOST_MEM_PAGESIZE), i * HOST_MEM_PAGESIZE + ALIGN_DOWN(physical_range.base, HOST_MEM_PAGESIZE), flags);
    }

    return OK(vmm_result_t, virtual_range);
}

void vmm_space_switch(vmm_space_t space)
{
    asm volatile("mov %0, %%cr3" ::"a"(mmap_io_to_phys((uintptr_t)space)));
}

// later we may use an other initializer that fork the higher kernel vmm entry
static void vmm_table_initialize_kernel(vmm_space_t target, struct handover_mmap const *memory_map)
{
    log("VMM: loading kernel map initial 32M");
    // load first 32M
    vmm_map(target,
            (vmm_range_t){
                .base = 0,
                .size = 0x100000000},
            (pmm_range_t){
                .base = 0,
                .size = 0x100000000},
            BR_MEM_WRITABLE);

    vmm_map(target,
            (vmm_range_t){
                .base = mmap_phys_to_kernel(0),
                .size = 0x2000000},
            (pmm_range_t){
                .base = (0),
                .size = 0x2000000},
            BR_MEM_WRITABLE);

    vmm_map(target,
            (vmm_range_t){
                .base = mmap_phys_to_io(0),
                .size = 0x2000000},
            (pmm_range_t){
                .base = (0),
                .size = 0x2000000},
            BR_MEM_WRITABLE);

    vmm_space_switch(target);

    log("VMM: loading kernel memory map");

    for (size_t i = 0; i < memory_map->size; i++)
    {

        log("VMM: loading kernel memory map {x}/{x} ({x} - {x})", i, memory_map->size, memory_map->entries[i].base, memory_map->entries[i].base + memory_map->entries[i].length);
        //log("    {x}-{x}", (uintptr_t)memory_map->entries[i].base, (uintptr_t)(memory_map->entries[i].base + memory_map->entries[i].size - 1));

        if (memory_map->entries[i].type == HANDOVER_MMAP_KERNEL_MODULE)
        {

            vmm_map(target,
                    (vmm_range_t){
                        .base = mmap_phys_to_kernel(memory_map->entries[i].base),
                        .size = memory_map->entries[i].length},
                    (pmm_range_t){
                        .base = (memory_map->entries[i].base),
                        .size = memory_map->entries[i].length},
                    BR_MEM_WRITABLE);
        }

        vmm_map(target,
                (vmm_range_t){
                    .base = mmap_phys_to_io(memory_map->entries[i].base),
                    .size = memory_map->entries[i].length},
                (pmm_range_t){
                    .base = (memory_map->entries[i].base),
                    .size = memory_map->entries[i].length},
                BR_MEM_WRITABLE);
    }
}

void vmm_initialize_kernel(struct handover const *handover)
{
    kernel_memory_map = (vmm_space_t)mmap_phys_to_io((pmm_alloc(HOST_MEM_PAGESIZE).ok).base);
    mem_set(kernel_memory_map, 0, HOST_MEM_PAGESIZE);

    vmm_table_initialize_kernel(kernel_memory_map, &handover->mmap);
    vmm_space_switch(kernel_memory_map);

    log("VMM: loaded kernel memory map");
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