#include <brutal/debug.h>
#include <brutal/sync.h>
#include "kernel/heap.h"
#include "kernel/mmap.h"
#include "kernel/vmm.h"
#include "kernel/x86_64/asm.h"
#include "kernel/x86_64/paging.h"

static Pml *kernel_pml = nullptr;
static Lock vmm_lock;

VmmResult vmm_flush(MAYBE_UNUSED VmmSpace space, VmmRange virtual_range)
{
    for (size_t i = 0; i < (virtual_range.size / MEM_PAGE_SIZE); i++)
    {
        asm_invlpg(((virtual_range.base / MEM_PAGE_SIZE) + i) * MEM_PAGE_SIZE);
    }

    return OK(VmmResult, virtual_range);
}

static VmmResult vmm_get_pml(Pml *table, size_t idx)
{
    PmlEntry entry = table->entries[idx];

    if (!entry.present)
    {
        return ERR(VmmResult, BR_BAD_ADDRESS);
    }

    VmmRange range = {
        mmap_phys_to_io(PML_GET_ADDR(entry)),
        MEM_PAGE_SIZE,
    };

    return OK(VmmResult, range);
}

static VmmResult vmm_get_pml_or_alloc(Pml *table, size_t idx, size_t flags)
{
    PmlEntry entry = table->entries[idx];

    if (entry.present)
    {
        VmmRange range = {
            mmap_phys_to_io(PML_GET_ADDR(entry)),
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

static void vmm_load_memory_map(VmmSpace target, HandoverMmap const *memory_map)
{
    log$("Loading kernel memory map...");

    log$("Mapping I/O space...");
    vmm_map(target,
            (VmmRange){
                .base = mmap_phys_to_io(0),
                .size = GiB(4),
            },
            (PmmRange){
                .base = 0,
                .size = GiB(4),
            },
            BR_MEM_WRITABLE);

    log$("Mapping Memory Map...");

    for (size_t i = 0; i < memory_map->size; i++)
    {
        HandoverMmapEntry entry = memory_map->entries[i];

        log$("Loading kernel memory map {}/{} ({x} - {x})",
             i + 1,
             memory_map->size,
             entry.base,
             entry.base + entry.len);

        if (entry.type == HANDOVER_MMAP_KERNEL_MODULE)
        {
            log$(" - Mapped to kernel");
            vmm_map(target,
                    (VmmRange){
                        .base = mmap_phys_to_kernel(ALIGN_DOWN(entry.base, MEM_PAGE_SIZE)),
                        .size = ALIGN_UP(entry.len, MEM_PAGE_SIZE)},
                    (PmmRange){
                        .base = ALIGN_DOWN(entry.base, MEM_PAGE_SIZE),
                        .size = ALIGN_UP(entry.len, MEM_PAGE_SIZE)},
                    BR_MEM_WRITABLE);
        }

        if (entry.base >= GiB(4)) // The bottom 4Gio are already mapped
        {
            log$(" - Mapped to IO");
            vmm_map(target,
                    (VmmRange){
                        .base = mmap_phys_to_io(ALIGN_DOWN(entry.base, MEM_PAGE_SIZE)),
                        .size = ALIGN_UP(entry.len, MEM_PAGE_SIZE)},
                    (PmmRange){
                        .base = ALIGN_DOWN(entry.base, MEM_PAGE_SIZE),
                        .size = ALIGN_UP(entry.len, MEM_PAGE_SIZE)},
                    BR_MEM_WRITABLE);
        }
    }

    vmm_space_switch(target);
}

void vmm_initialize(Handover const *handover)
{
    HeapRange heap_result = UNWRAP(heap_alloc(MEM_PAGE_SIZE));

    kernel_pml = (Pml *)heap_result.base;
    mem_set(kernel_pml, 0, MEM_PAGE_SIZE);

    vmm_load_memory_map(kernel_pml, &handover->mmap);
    vmm_space_switch(kernel_pml);

    log$("Loaded kernel memory map!");
}

VmmSpace vmm_space_create(void)
{
    LOCK_RETAINER(&vmm_lock);

    HeapRange heap_result = UNWRAP(heap_alloc(MEM_PAGE_SIZE));

    VmmSpace vmm_address_space = (VmmSpace)heap_result.base;
    mem_set(vmm_address_space, 0, MEM_PAGE_SIZE);

    Pml *pml_table = (Pml *)vmm_address_space;

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

static void vmm_space_destroy_pml(int level, Pml *pml, size_t start, size_t end)
{
    if (level < 1)
    {
        return;
    }

    for (size_t i = start; i < end; i++)
    {
        VmmResult vmm_result = vmm_get_pml(pml, i);

        if (!vmm_result.succ)
        {
            continue;
        }

        vmm_space_destroy_pml(level - 1, (Pml *)UNWRAP(vmm_result).base, 0, 512);
    }

    heap_free((HeapRange){(uintptr_t)pml, MEM_PAGE_SIZE});
}

void vmm_space_destroy(VmmSpace space)
{
    vmm_space_destroy_pml(4, (Pml *)space, 0, 255);
}

void vmm_space_switch(VmmSpace space)
{
    LOCK_RETAINER(&vmm_lock);

    asm_write_cr3(mmap_io_to_phys((uintptr_t)space));
}

VmmSpace vmm_kernel_space(void)
{
    return (VmmSpace)kernel_pml;
}

static VmmResult vmm_map_page(Pml *pml4, uintptr_t virtual_page, uintptr_t physical_page, BrMemFlags flags)
{
    VmmRange pml3_range = TRY(VmmResult, vmm_get_pml_or_alloc(pml4, PML4_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER));
    Pml *pml3 = (Pml *)(pml3_range.base);

    VmmRange pml2_range = TRY(VmmResult, vmm_get_pml_or_alloc(pml3, PML3_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER));
    Pml *pml2 = (Pml *)(pml2_range.base);

    VmmRange pml1_range = TRY(VmmResult, vmm_get_pml_or_alloc(pml2, PML2_GET_INDEX(virtual_page), flags | BR_MEM_WRITABLE | BR_MEM_USER));
    Pml *pml1 = (Pml *)(pml1_range.base);

    PmlEntry *entry = &pml1->entries[PML1_GET_INDEX(virtual_page)];

    if (entry->present)
    {
        panic$("{#x} is already mapped to {#x}", virtual_page, (uintptr_t)PML_GET_ADDR(*entry));
    }

    *entry = pml_make_entry(physical_page, flags);

    VmmRange range = {virtual_page, MEM_PAGE_SIZE};

    return OK(VmmResult, range);
}

static VmmResult vmm_unmap_page(Pml *pml4, uintptr_t virtual_page)
{
    VmmRange pml3_range = TRY(VmmResult, vmm_get_pml(pml4, PML4_GET_INDEX(virtual_page)));
    Pml *pml3 = (Pml *)(pml3_range.base);

    VmmRange pml2_range = TRY(VmmResult, vmm_get_pml(pml3, PML3_GET_INDEX(virtual_page)));
    Pml *pml2 = (Pml *)(pml2_range.base);

    VmmRange pml1_range = TRY(VmmResult, vmm_get_pml(pml2, PML2_GET_INDEX(virtual_page)));
    Pml *pml1 = (Pml *)(pml1_range.base);

    PmlEntry *entry = &pml1->entries[PML1_GET_INDEX(virtual_page)];
    assert_truth((bool)entry->present);

    *entry = pml_clean_entry();

    VmmRange range = {virtual_page, MEM_PAGE_SIZE};
    return OK(VmmResult, range);
}

VmmResult vmm_map(VmmSpace space, VmmRange virtual_range, PmmRange physical_range, BrMemFlags flags)
{
    assert_truth(mem_is_range_page_aligned(virtual_range));
    assert_truth(mem_is_range_page_aligned(physical_range));

    LOCK_RETAINER(&vmm_lock);

    if (virtual_range.size != physical_range.size)
    {
        panic$("virtual_range.size must be equal to physical_range.size");
        return ERR(VmmResult, BR_BAD_ARGUMENTS);
    }

    for (size_t i = 0; i < (virtual_range.size / MEM_PAGE_SIZE); i++)
    {
        vmm_map_page(
            (Pml *)space,
            i * MEM_PAGE_SIZE + ALIGN_DOWN(virtual_range.base, MEM_PAGE_SIZE),
            i * MEM_PAGE_SIZE + ALIGN_DOWN(physical_range.base, MEM_PAGE_SIZE),
            flags);
    }

    return OK(VmmResult, virtual_range);
}

VmmResult vmm_unmap(VmmSpace space, VmmRange virtual_range)
{
    assert_truth(mem_is_range_page_aligned(virtual_range));

    LOCK_RETAINER(&vmm_lock);

    for (size_t i = 0; i < (virtual_range.size / MEM_PAGE_SIZE); i++)
    {
        vmm_unmap_page((Pml *)space, i * MEM_PAGE_SIZE + ALIGN_DOWN(virtual_range.base, MEM_PAGE_SIZE));
    }

    return OK(VmmResult, virtual_range);
}

PmmResult vmm_virt2phys(VmmSpace space, VmmRange virtual_range)
{
    LOCK_RETAINER(&vmm_lock);

    Pml *pml4 = (Pml *)space;

    VmmRange pml3_range = TRY(PmmResult, vmm_get_pml(pml4, PML4_GET_INDEX(virtual_range.base)));
    Pml *pml3 = (Pml *)(pml3_range.base);

    VmmRange pml2_range = TRY(PmmResult, vmm_get_pml(pml3, PML3_GET_INDEX(virtual_range.base)));
    Pml *pml2 = (Pml *)(pml2_range.base);

    VmmRange pml1_range = TRY(PmmResult, vmm_get_pml(pml2, PML2_GET_INDEX(virtual_range.base)));
    Pml *pml1 = (Pml *)(pml1_range.base);

    PmlEntry entry = pml1->entries[PML1_GET_INDEX(virtual_range.base)];
    PmmRange range = {PML_GET_ADDR(entry), virtual_range.size};

    return OK(PmmResult, range);
}
