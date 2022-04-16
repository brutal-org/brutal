#include <bal/boot/handover.h>
#include <brutal-debug>
#include <brutal-mem>

void handover_mmap_append(HandoverMmap *self, HandoverMmapEntry entry)
{
    if (self->size + 1 >= HANDOVER_MMAP_MAX_SIZE)
    {
        panic$("Memory map full!");
    }

    if (range_empty(entry.range))
    {
        return;
    }

    if (entry.type == HANDOVER_MMAP_FREE)
    {
        entry.range = range_align_smaller(entry.range, MEM_PAGE_SIZE);
    }
    else
    {
        entry.range = range_align_bigger(entry.range, MEM_PAGE_SIZE);
    }

    for (size_t i = 0; i < self->size; i++)
    {
        HandoverMmapEntry *e = &self->entries[i];

        if (range_touch(e->range, entry.range) && e->type == entry.type)
        {
            e->range = range_merge(e->range, entry.range);
            return;
        }
        else if (range_colide(e->range, entry.range))
        {
            if (entry.type < e->type)
            {
                swap$(e, &entry);
            }

            HandoverMmapType type = e->type;
            USizeRange lower = range_half_under(e->range, entry.range);
            USizeRange over = range_half_over(e->range, entry.range);

            *e = entry;

            handover_mmap_append(self, (HandoverMmapEntry){.range = lower, .type = type});
            handover_mmap_append(self, (HandoverMmapEntry){.range = over, .type = type});
            return;
        }
        else if (e->range.base > entry.range.base)
        {
            for (size_t j = self->size; j > i; j--)
            {
                self->entries[j] = self->entries[j - 1];
            }

            self->entries[i] = entry;
            self->size++;
            return;
        }
    }

    self->entries[self->size++] = entry;
}

uintptr_t handover_mmap_base(HandoverMmap const *memory_map)
{
    return memory_map->entries[0].base;
}

uintptr_t handover_mmap_end(HandoverMmap const *memory_map)
{
    return range_end(memory_map->entries[memory_map->size - 1]);
}
