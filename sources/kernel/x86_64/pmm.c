#include <brutal/debug.h>
#include <brutal/ds.h>
#include <brutal/mem.h>
#include <brutal/sync.h>
#include "kernel/arch.h"
#include "kernel/mmap.h"
#include "kernel/pmm.h"

static Bits pmm_bitmap = {};

static size_t best_bet_upper = 0;
static size_t best_bet_lower = (size_t)-1;

static size_t used_memory = 0;
static Lock pmm_lock = {};

static uintptr_t memory_map_get_highest_address(HandoverMmap const *memory_map)
{
    size_t len = memory_map->entries[memory_map->size - 1].len;
    size_t start = memory_map->entries[memory_map->size - 1].base;

    return ALIGN_UP(len + start, MEM_PAGE_SIZE);
}

static void pmm_bitmap_initialize(HandoverMmap const *memory_map)
{
    log$("Allocating memory bitmap...");

    size_t bitmap_target_size = memory_map_get_highest_address(memory_map) / MEM_PAGE_SIZE / 8;

    log$("A bitmap {}kib long is needed", bitmap_target_size / 1024);

    for (size_t i = 0; i < memory_map->size; i++)
    {
        HandoverMmapEntry entry = memory_map->entries[i];

        if (entry.type != HANDOVER_MMAP_FREE)
        {
            continue;
        }

        if (entry.len > bitmap_target_size)
        {
            log$("Allocated memory bitmap at {x}-{x}", entry.base, entry.base + bitmap_target_size - 1);

            bits_init(&pmm_bitmap, (void *)mmap_phys_to_io(entry.base), bitmap_target_size);
            break;
        }
    }

    bits_fill(&pmm_bitmap, PMM_USED);
}

static void pmm_load_memory_map(HandoverMmap const *memory_map)
{
    size_t available_memory = 0;

    for (size_t i = 0; i < memory_map->size; i++)
    {
        HandoverMmapEntry entry = memory_map->entries[i];

        if (entry.type != HANDOVER_MMAP_FREE)
        {
            continue;
        }

        PmmRange entry_range = {entry.base, entry.len};

        pmm_unused(range_align_lower(entry_range, MEM_PAGE_SIZE));

        available_memory += entry_range.size;
    }

    log$("Available Memory: {}kib", available_memory / KiB(1));
}

void pmm_initialize(Handover const *handover)
{
    pmm_bitmap_initialize(&handover->mmap);
    pmm_load_memory_map(&handover->mmap);

    // we set the first page used, as the page 0 is NULL
    pmm_used((PmmRange){(uintptr_t)0, MEM_PAGE_SIZE * 32});
    used_memory = 0;
    pmm_used((PmmRange){mmap_io_to_phys((uintptr_t)pmm_bitmap.data), pmm_bitmap.size});
}

PmmResult pmm_alloc(size_t size, bool upper)
{
    LOCK_RETAINER(&pmm_lock);

    assert_truth(mem_is_size_page_aligned(size));

    used_memory += size;

    size_t page_size = size / MEM_PAGE_SIZE;
    USizeRange page_range = bits_find_free(&pmm_bitmap, upper ? best_bet_upper : best_bet_lower, page_size, upper);

    if (range_empty(page_range))
    {
        if (upper)
        {
            best_bet_upper = -1;
        }
        else
        {
            best_bet_lower = 0;
        }

        page_range = bits_find_free(&pmm_bitmap, upper ? -1 : 0, page_size, upper);
    }

    if (!range_any(page_range))
    {
        log$("pmm_alloc({}): out of memory!", size);
        return ERR(PmmResult, BR_OUT_OF_MEMORY);
    }

    if (upper)
    {
        best_bet_upper = range_begin(page_range);
    }
    else
    {
        best_bet_lower = range_end(page_range);
    }

    bits_set_range(&pmm_bitmap, page_range, PMM_USED);

    PmmRange pmm_range = range_mul(range$(PmmRange, page_range), MEM_PAGE_SIZE);

    return OK(PmmResult, pmm_range);
}

PmmResult pmm_used(PmmRange range)
{
    LOCK_RETAINER(&pmm_lock);

    assert_truth(mem_is_range_page_aligned(range));

    USizeRange page_range = range$(USizeRange, range_div(range, MEM_PAGE_SIZE));

    bits_set_range(&pmm_bitmap, page_range, PMM_USED);

    return OK(PmmResult, range);
}

PmmResult pmm_unused(PmmRange range)
{
    LOCK_RETAINER(&pmm_lock);

    assert_truth(mem_is_range_page_aligned(range));

    used_memory -= range.size;

    if (range.base == 0)
    {
        return ERR(PmmResult, BR_BAD_ADDRESS);
    }

    USizeRange page_range = range$(USizeRange, range_div(range, MEM_PAGE_SIZE));

    bits_set_range(&pmm_bitmap, page_range, PMM_UNUSED);

    if (best_bet_upper < range_end(page_range))
    {
        best_bet_upper = range_end(page_range);
    }

    if (best_bet_lower > range_begin(page_range))
    {
        best_bet_lower = range_begin(page_range);
    }

    return OK(PmmResult, range);
}
