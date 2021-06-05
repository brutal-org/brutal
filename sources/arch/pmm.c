#include <library/base/macros.h>
#include <library/log.h>
#include <library/mem.h>

#include "arch.h"
#include "arch/mmap.h"
#include "arch/pmm.h"

struct bitmap pmm_bitmap = {};

size_t last_free_bitmap_entry = 0;
size_t available_memory = 0;
size_t bitmap_target_size = 0;

static uintptr_t memory_map_get_highest_address(const struct handover_mmap *memory_map)
{
    size_t length = memory_map->mmap_table[memory_map->size - 1].length;
    size_t start = memory_map->mmap_table[memory_map->size - 1].base;

    return length + start;
}

static void pmm_bitmap_find_addr(const struct handover_mmap *memory_map)
{
    for (size_t i = 0; i < memory_map->size; i++)
    {
        if (memory_map->mmap_table[i].type != HANDOVER_MMAP_FREE)
        {
            continue;
        }

        // + 2 PAGE FOR SECURITY
        if (memory_map->mmap_table[i].length > ((bitmap_target_size / 8) + (HOST_MEM_PAGESIZE * 2)))
        {
            pmm_bitmap =
                bitmap((void *)memory_map->mmap_table[i].base, bitmap_target_size);
            return;
        }
    }
}

static void pmm_bitmap_clear()
{
    bitmap_fill(&pmm_bitmap, true);
    last_free_bitmap_entry = 0;
}

void pmm_initialize_memory_map(const struct handover_mmap *memory_map)
{
    for (size_t i = 0; i < memory_map->size; i++)
    {
        size_t start = ALIGN_UP(memory_map->mmap_table[i].base, HOST_MEM_PAGESIZE);
        size_t size = ALIGN_DOWN(memory_map->mmap_table[i].length, HOST_MEM_PAGESIZE);

        log("memory map: type: {x} {x}-{x}", memory_map->mmap_table[i].type, start, start + size);

        if (memory_map->mmap_table[i].type != HANDOVER_MMAP_FREE)
        {
            continue;
        }

        pmm_free(start, size / HOST_MEM_PAGESIZE);

        available_memory += size / HOST_MEM_PAGESIZE;
    }
}

static size_t pmm_bitmap_find_free(size_t page_count)
{
    size_t free_entry_count_in_a_row = 0;
    size_t free_entry_start = 0;

    for (size_t i = last_free_bitmap_entry; i < pmm_bitmap.size; i++)
    {
        if (!bitmap_get(&pmm_bitmap, i))
        {
            if (free_entry_count_in_a_row == 0)
            {
                free_entry_start = i;
            }

            free_entry_count_in_a_row++;
        }
        else
        {
            free_entry_count_in_a_row = 0;
            free_entry_start = 0;
        }

        if (page_count <= free_entry_count_in_a_row)
        {
            last_free_bitmap_entry = free_entry_start + page_count;
            return free_entry_start;
        }
    }

    if (last_free_bitmap_entry == 0)
    {
        return 0;
    }
    else
    {
        // retry but from 0 instead of from the last free entry
        last_free_bitmap_entry = 0;
        return pmm_bitmap_find_free(page_count);
    }
}

uintptr_t pmm_alloc(size_t page_count)
{
    size_t page = pmm_bitmap_find_free(page_count);
    bitmap_set_range(&pmm_bitmap, page, page_count, true);
    return page * HOST_MEM_PAGESIZE;
}

uintptr_t pmm_alloc_zero(size_t page_count)
{
    uintptr_t result = pmm_alloc(page_count);

    if (result)
    {
        mem_set((void *)(result + MMAP_KERNEL_BASE), 0, page_count * HOST_MEM_PAGESIZE);
    }

    return result;
}

int pmm_free(uintptr_t addr, size_t page_count)
{
    if (addr == 0)
    {
        return -1;
    }

    size_t page_idx = addr / HOST_MEM_PAGESIZE;

    bitmap_set_range(&pmm_bitmap, page_idx, page_count, false);

    last_free_bitmap_entry = page_idx;
    return 0;
}

void pmm_initialize(const struct handover_mmap *memory_map)
{
    bitmap_target_size = memory_map_get_highest_address(memory_map) /
                         HOST_MEM_PAGESIZE; // load bitmap size

    pmm_bitmap_find_addr(memory_map);

    pmm_bitmap_clear();

    pmm_initialize_memory_map(memory_map);

    // we set the first page used, as the page 0 is NULL
    bitmap_set(&pmm_bitmap, 0, true);

    // we set where the bitmap is located to used
    bitmap_set_range(&pmm_bitmap, (uintptr_t)pmm_bitmap.data / HOST_MEM_PAGESIZE, (pmm_bitmap.size / HOST_MEM_PAGESIZE) / 8 + 2, true);
}