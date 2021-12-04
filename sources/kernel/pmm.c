#include <brutal/debug.h>
#include <brutal/ds.h>
#include <brutal/mem.h>
#include <brutal/sync.h>
#include "kernel/arch.h"
#include "kernel/mmap.h"
#include "kernel/pmm.h"

static Lock _lock = {};
static Bits _bitmap = {};
static size_t _bestbet_upper = 0;
static size_t _bestbet_lower = (size_t)-1;
static size_t _used_memory = 0;
static size_t _available_memory = 0;
static PmmRange _usable_range = {};

static BitsRange addr2pages(PmmRange range)
{
    range.base -= _usable_range.base;
    range = range_div(range, MEM_PAGE_SIZE);

    return range$(BitsRange, range);
}

static PmmRange pages2addr(BitsRange range)
{
    range = range_mul(range, MEM_PAGE_SIZE);
    range.base += _usable_range.base;

    return range$(PmmRange, range);
}

static void pmm_bitmap_init(HandoverMmap const *memory_map)
{
    _usable_range = range_from_start_and_end(PmmRange, handover_mmap_base(memory_map), handover_mmap_end(memory_map));
    log$("Allocating memory bitmap...");

    size_t bitmap_size = ALIGN_UP(_usable_range.size / MEM_PAGE_SIZE / 8, MEM_PAGE_SIZE);

    log$("A bitmap {}kib long is needed", bitmap_size / 1024);

    for (size_t i = 0; i < memory_map->size; i++)
    {
        HandoverMmapEntry entry = memory_map->entries[i];

        if (entry.type != HANDOVER_MMAP_FREE)
        {
            continue;
        }

        if (entry.size > bitmap_size)
        {
            log$("Allocated memory bitmap at {x}-{x}", entry.base, entry.base + bitmap_size - 1);

            bits_init(&_bitmap, (void *)mmap_phys_to_io(entry.base), bitmap_size);
            break;
        }
    }

    bits_fill(&_bitmap, PMM_USED);
}

static void pmm_mmap_load(HandoverMmap const *memory_map)
{

    for (size_t i = 0; i < memory_map->size; i++)
    {
        HandoverMmapEntry entry = memory_map->entries[i];

        if (entry.type == HANDOVER_MMAP_FREE)
        {
            PmmRange entry_range = {entry.base, entry.size};

            pmm_unused(range_align_smaller(entry_range, MEM_PAGE_SIZE));

            _available_memory += entry_range.size;
        }
    }

    log$("Available Memory: {}kib", _available_memory / KiB(1));
}

void pmm_initialize(Handover const *handover)
{
    pmm_bitmap_init(&handover->mmap);
    pmm_mmap_load(&handover->mmap);

    // we set the first part of memory used
    pmm_used((PmmRange){(uintptr_t)_usable_range.base, MEM_PAGE_SIZE * 32});
    pmm_used((PmmRange){mmap_io_to_phys((uintptr_t)_bitmap.data), _bitmap.size});
}

PmmResult pmm_alloc(size_t size, bool upper)
{
    LOCK_RETAINER(&_lock);

    assert_truth(mem_is_size_page_aligned(size));

    _used_memory += size;

    size_t page_size = size / MEM_PAGE_SIZE;
    BitsRange page_range = bits_find_free(&_bitmap, upper ? _bestbet_upper : _bestbet_lower, page_size, upper);

    if (range_empty(page_range))
    {
        if (upper)
        {
            _bestbet_upper = -1;
        }
        else
        {
            _bestbet_lower = 0;
        }

        page_range = bits_find_free(&_bitmap, upper ? -1 : 0, page_size, upper);
    }

    if (!range_any(page_range))
    {
        log$("pmm_alloc({}): out of memory!", size);
        return ERR(PmmResult, BR_OUT_OF_MEMORY);
    }

    if (upper)
    {
        _bestbet_upper = range_begin(page_range);
    }
    else
    {
        _bestbet_lower = range_end(page_range);
    }

    bits_set_range(&_bitmap, page_range, PMM_USED);

    PmmRange pmm_range = pages2addr(page_range);

    return OK(PmmResult, pmm_range);
}

PmmResult pmm_used(PmmRange range)
{
    LOCK_RETAINER(&_lock);

    assert_truth(mem_is_range_page_aligned(range));

    BitsRange page_range = addr2pages(range);

    bits_set_range(&_bitmap, page_range, PMM_USED);

    return OK(PmmResult, range);
}

PmmResult pmm_unused(PmmRange range)
{
    LOCK_RETAINER(&_lock);

    assert_truth(mem_is_range_page_aligned(range));

    _used_memory -= range.size;

    if (range.base == 0)
    {
        return ERR(PmmResult, BR_BAD_ADDRESS);
    }

    BitsRange page_range = addr2pages(range);

    bits_set_range(&_bitmap, page_range, PMM_UNUSED);

    if (_bestbet_upper < range_end(page_range))
    {
        _bestbet_upper = range_end(page_range);
    }

    if (_bestbet_lower > range_begin(page_range))
    {
        _bestbet_lower = range_begin(page_range);
    }

    return OK(PmmResult, range);
}
