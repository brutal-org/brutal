#include <brutal/base.h>
#include <brutal/ds.h>
#include <brutal/log.h>
#include <brutal/mem.h>
#include <brutal/sync.h>
#include "arch/arch.h"
#include "arch/pmm.h"
#include "arch/x86_64/memory/mmap.h"

static Bitmap pmm_bitmap = {};
static size_t best_bet = 0;
static size_t used_memory = 0;
static Lock pmm_lock;

static uintptr_t memory_map_get_highest_address(struct handover_mmap const *memory_map)
{
    size_t length = memory_map->entries[memory_map->size - 1].length;
    size_t start = memory_map->entries[memory_map->size - 1].base;

    return ALIGN_UP(length + start, HOST_MEM_PAGESIZE);
}

static void pmm_bitmap_initialize(struct handover_mmap const *memory_map)
{
    log("Allocating memory bitmap...");

    size_t bitmap_target_size = memory_map_get_highest_address(memory_map) / HOST_MEM_PAGESIZE / 8;

    log("A bitmap {}kib long is needed", bitmap_target_size / 1024);

    for (size_t i = 0; i < memory_map->size; i++)
    {
        auto entry = &memory_map->entries[i];

        if (entry->type != HANDOVER_MMAP_FREE)
        {
            continue;
        }

        if (entry->length > bitmap_target_size)
        {
            log("Allocated memory bitmap at {x}-{x}", entry->base, entry->base + bitmap_target_size - 1);

            bitmap_init(&pmm_bitmap, (void *)mmap_phys_to_io(entry->base), bitmap_target_size);
            break;
        }
    }

    bitmap_fill(&pmm_bitmap, PMM_USED);
}

static void pmm_load_memory_map(struct handover_mmap const *memory_map)
{
    log("Memory map:");

    size_t available_memory = 0;

    for (size_t i = 0; i < memory_map->size; i++)
    {
        size_t base = ALIGN_UP(memory_map->entries[i].base, HOST_MEM_PAGESIZE);
        size_t size = ALIGN_DOWN(memory_map->entries[i].length, HOST_MEM_PAGESIZE);

        log("    type: {x} {x}-{x}", memory_map->entries[i].type, base, base + size - 1);

        if (memory_map->entries[i].type != HANDOVER_MMAP_FREE)
        {
            continue;
        }

        pmm_unused((PmmRange){base, size});

        available_memory += size;
    }

    log("Available Memory: {}kib", available_memory / 1024);
}

void pmm_initialize(struct handover const *handover)
{
    pmm_bitmap_initialize(&handover->mmap);
    pmm_load_memory_map(&handover->mmap);

    // we set the first page used, as the page 0 is NULL
    pmm_used((PmmRange){(uintptr_t)0, HOST_MEM_PAGESIZE * 32});
    used_memory = 0;
    pmm_used((PmmRange){mmap_io_to_phys((uintptr_t)pmm_bitmap.data), pmm_bitmap.size});
}

PmmResult pmm_alloc(size_t size)
{
    LOCK_RETAINER(&pmm_lock);

    used_memory += size;
    // log("pmm_alloc(): {} (total: {x})", size, used_memory);

    auto page_size = size / HOST_MEM_PAGESIZE;
    auto page_range = bitmap_find_range(&pmm_bitmap, best_bet, page_size, PMM_UNUSED);

    if (range_empty(page_range))
    {
        best_bet = 0;
        page_range = bitmap_find_range(&pmm_bitmap, 0, page_size, PMM_UNUSED);
    }

    if (range_any(page_range))
    {
        best_bet = range_end(page_range);

        bitmap_set_range(&pmm_bitmap, page_range, PMM_USED);

        auto pmm_range = range_cast(PmmRange, page_range);

        pmm_range.base *= HOST_MEM_PAGESIZE;
        pmm_range.size *= HOST_MEM_PAGESIZE;

        return OK(PmmResult, pmm_range);
    }
    else
    {
        log("pmm_alloc(): {} error out of memory", size);
        return ERR(PmmResult, BR_ERR_OUT_OF_MEMORY);
    }
}

PmmResult pmm_used(PmmRange range)
{
    LOCK_RETAINER(&pmm_lock);

    log("pmm_used(): {x}-{x}...", range.base, range_end(range) - 1);

    size_t page_base = range.base / HOST_MEM_PAGESIZE;
    size_t page_size = range.size / HOST_MEM_PAGESIZE;
    auto page_range = (BitmapRange){page_base, page_size};

    bitmap_set_range(&pmm_bitmap, page_range, PMM_USED);

    return OK(PmmResult, range);
}

PmmResult pmm_unused(PmmRange range)
{
    LOCK_RETAINER(&pmm_lock);

    used_memory -= range.size;
    log("pmm_unused(): {x}-{x}...", range.base, range_end(range) - 1);

    if (range.base == 0)
    {
        return ERR(PmmResult, BR_ERR_BAD_ADDRESS);
    }

    size_t page_base = range.base / HOST_MEM_PAGESIZE;
    size_t page_size = range.size / HOST_MEM_PAGESIZE;
    auto page_range = (BitmapRange){page_base, page_size};

    bitmap_set_range(&pmm_bitmap, page_range, PMM_UNUSED);

    best_bet = (page_range.base);

    return OK(PmmResult, range);
}
