#include <brutal/log.h>
#include <brutal/sync.h>
#include "kernel/heap.h"
#include "kernel/mmap.h"
#include "kernel/vmm.h"
#include "kernel/x86_64/asm.h"
#include "kernel/x86_64/paging.h"

static struct pml *kernel_pml = nullptr;
static Lock vmm_lock;

static VmmResult vmm_get_pml(struct pml *table, size_t idx)
{
    auto entry = table->entries[idx];

    if (entry.present)
    {
        VmmRange range = {
            mmap_phys_to_io(entry.physical << 12),
            MEM_PAGE_SIZE,
        };

        return OK(VmmResult, range);
    }
    else
    {
        return ERR(VmmResult, BR_BAD_ADDRESS);
    }
}

static VmmResult vmm_get_pml_or_alloc(struct pml *table, size_t idx, size_t flags)
{
    auto entry = table->entries[idx];

    if (entry.present)
    {
        VmmRange range = {
            mmap_phys_to_io(entry.physical << 12),
            MEM_PAGE_SIZE,
        };

        return OK(VmmResult, range);
    }
    else
    {
        uintptr_t target = TRY(VmmResult, heap_alloc(MEM_PAGE_SIZE)).base;

        mem_set((void *)target, 0, MEM_PAGE_SIZE);
        table->entries[idx] = pml_make_entry(mmap_io_to_phys(target), flags);

        VmmRange range = {target, MEM_PAGE_SIZE};
        return OK(VmmResult, range);
    }
}

static VmmResult vmm_map_page(struct pml *pml4, uintptr_t virtual_page, uintptr_t physical_page, BrMemFlags flags)
{
    auto pml3_range = TRY(VmmResult, vmm_get_pml_or_alloc(pml4, PML4_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER));
    struct pml *pml3 = (struct pml *)(pml3_range.base);

    auto pml2_range = TRY(VmmResult, vmm_get_pml_or_alloc(pml3, PML3_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER));
    struct pml *pml2 = (struct pml *)(pml2_range.base);

    auto pml1_range = TRY(VmmResult, vmm_get_pml_or_alloc(pml2, PML2_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER));
    struct pml *pml1 = (struct pml *)(pml1_range.base);

    if (pml1->entries[PML1_GET_INDEX(virtual_page)].present)
    {
        panic("{#p} is already mapped to {#p}", virtual_page, (uint64_t)pml2->entries[PML1_GET_INDEX(virtual_page)].physical << 12);
    }

    pml1->entries[PML1_GET_INDEX(virtual_page)] = pml_make_entry(physical_page, flags);

    VmmRange range = {virtual_page, MEM_PAGE_SIZE};
    return OK(VmmResult, range);
}

static VmmResult vmm_unmap_page(struct pml *pml4, uintptr_t virtual_page)
{
    auto pml3_range = TRY(VmmResult, vmm_get_pml(pml4, PML4_GET_INDEX(virtual_page)));
    struct pml *pml3 = (struct pml *)(pml3_range.base);

    auto pml2_range = TRY(VmmResult, vmm_get_pml(pml3, PML3_GET_INDEX(virtual_page)));
    struct pml *pml2 = (struct pml *)(pml2_range.base);

    auto pml1_range = TRY(VmmResult, vmm_get_pml(pml2, PML2_GET_INDEX(virtual_page)));
    struct pml *pml1 = (struct pml *)(pml1_range.base);

    assert_truth(pml1->entries[PML1_GET_INDEX(virtual_page)].present == 1);

    pml1->entries[PML1_GET_INDEX(virtual_page)] = pml_clean_entry();

    VmmRange range = {virtual_page, MEM_PAGE_SIZE};
    return OK(VmmResult, range);
}

static void vmm_load_memory_map(VmmSpace target, HandoverMmap const *memory_map)
{
    log("Loading kernel memory map...");

    log("Mapping I/O space...");
    vmm_map(target,
            (VmmRange){
                .base = mmap_phys_to_io(0),
                .size = 0xffffffff,
            },
            (PmmRange){
                .base = 0,
                .size = 0xffffffff,
            },
            BR_MEM_WRITABLE);

    for (size_t i = 0; i < memory_map->size; i++)
    {
        auto entry = memory_map->entries[i];

        log("Loading kernel memory map {}/{} ({x} - {x})",
            i + 1,
            memory_map->size,
            entry.base,
            entry.base + entry.length);

        if (entry.type == HANDOVER_MMAP_KERNEL_MODULE)
        {
            vmm_map(target,
                    (VmmRange){
                        .base = mmap_phys_to_kernel(ALIGN_DOWN(entry.base, MEM_PAGE_SIZE)),
                        .size = ALIGN_UP(entry.length, MEM_PAGE_SIZE) + MEM_PAGE_SIZE},
                    (PmmRange){
                        .base = ALIGN_DOWN(entry.base, MEM_PAGE_SIZE),
                        .size = ALIGN_UP(entry.length, MEM_PAGE_SIZE) + MEM_PAGE_SIZE},
                    BR_MEM_WRITABLE);
        }

        if (entry.base > 0xffffffff) // The bottom 4Gio are already mapped
        {
            vmm_map(target,
                    (VmmRange){
                        .base = mmap_phys_to_io(ALIGN_DOWN(entry.base, MEM_PAGE_SIZE)),
                        .size = ALIGN_UP(entry.length, MEM_PAGE_SIZE) + MEM_PAGE_SIZE},
                    (PmmRange){
                        .base = ALIGN_DOWN(entry.base, MEM_PAGE_SIZE),
                        .size = ALIGN_UP(entry.length, MEM_PAGE_SIZE) + MEM_PAGE_SIZE},
                    BR_MEM_WRITABLE);
        }
    }

    vmm_space_switch(target);
}

void vmm_initialize(Handover const *handover)
{
    auto heap_result = heap_alloc(MEM_PAGE_SIZE);

    kernel_pml = (struct pml *)UNWRAP(heap_result).base;
    mem_set(kernel_pml, 0, MEM_PAGE_SIZE);

    vmm_load_memory_map(kernel_pml, &handover->mmap);
    vmm_space_switch(kernel_pml);

    log("Loaded kernel memory map!");
}

VmmSpace vmm_space_create(void)
{
    LOCK_RETAINER(&vmm_lock);

    auto heap_result = heap_alloc(MEM_PAGE_SIZE);

    VmmSpace vmm_address_space = (VmmSpace)UNWRAP(heap_result).base;
    mem_set(vmm_address_space, 0, MEM_PAGE_SIZE);

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

static void vmm_space_destroy_pml(int level, struct pml *pml, size_t start, size_t end)
{
    if (level < 1)
    {
        return;
    }

    for (size_t i = start; i < end; i++)
    {
        auto vmm_result = vmm_get_pml(pml, i);

        if (!vmm_result.success)
        {
            continue;
        }

        vmm_space_destroy_pml(level - 1, (struct pml *)UNWRAP(vmm_result).base, 0, 512);
    }

    heap_free((HeapRange){(uintptr_t)pml, MEM_PAGE_SIZE});
}

void vmm_space_destroy(VmmSpace space)
{
    vmm_space_destroy_pml(4, (struct pml *)space, 0, 255);
}

void vmm_space_switch(VmmSpace space)
{
    LOCK_RETAINER(&vmm_lock);

    asm_write_cr3(mmap_io_to_phys((uintptr_t)space));
}

VmmResult vmm_map(VmmSpace space, VmmRange virtual_range, PmmRange physical_range, BrMemFlags flags)
{
    LOCK_RETAINER(&vmm_lock);

    log("vmm_map: {#p}-{#p} {#p}", virtual_range.base, virtual_range.base + virtual_range.size - 1, physical_range.base);

    if (virtual_range.size != physical_range.size)
    {
        panic("virtual_range.size must be equal to physical_range.size");
        return ERR(VmmResult, BR_BAD_ARGUMENTS);
    }

    for (size_t i = 0; i < (virtual_range.size / MEM_PAGE_SIZE); i++)
    {
        vmm_map_page(
            space,
            i * MEM_PAGE_SIZE + ALIGN_DOWN(virtual_range.base, MEM_PAGE_SIZE),
            i * MEM_PAGE_SIZE + ALIGN_DOWN(physical_range.base, MEM_PAGE_SIZE),
            flags);
    }

    return OK(VmmResult, virtual_range);
}

VmmResult vmm_unmap(VmmSpace space, VmmRange virtual_range)
{
    for (size_t i = 0; i < (virtual_range.size / MEM_PAGE_SIZE); i++)
    {
        vmm_unmap_page(space, i * MEM_PAGE_SIZE + ALIGN_DOWN(virtual_range.base, MEM_PAGE_SIZE));
    }

    return OK(VmmResult, virtual_range);
}

PmmResult vmm_virt2phys(VmmSpace space, VmmRange virtual_range)
{
    LOCK_RETAINER(&vmm_lock);

    struct pml *pml4 = space;

    auto pml3_range = TRY(PmmResult, vmm_get_pml(pml4, PML4_GET_INDEX(virtual_range.base)));
    struct pml *pml3 = (struct pml *)(pml3_range.base);

    auto pml2_range = TRY(PmmResult, vmm_get_pml(pml3, PML3_GET_INDEX(virtual_range.base)));
    struct pml *pml2 = (struct pml *)(pml2_range.base);

    auto pml1_range = TRY(PmmResult, vmm_get_pml(pml2, PML2_GET_INDEX(virtual_range.base)));
    struct pml *pml1 = (struct pml *)(pml1_range.base);

    auto entry = pml1->entries[PML1_GET_INDEX(virtual_range.base)];
    PmmRange range = {entry.physical << 12, virtual_range.size};

    return OK(PmmResult, range);
}

VmmSpace vmm_kernel_space(void)
{
    return (VmmSpace)kernel_pml;
}
