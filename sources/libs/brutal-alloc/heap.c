#include <brutal-debug>
#include <embed/mem.h>

#include "heap.h"

#define ALLOC_HEAP_ALIGN (64)
#define ALLOC_HEAP_MAGIC (0xc001c0de)
#define ALLOC_HEAP_DEAD (0xdeaddead)

static HeapMajor *major_block_create(size_t size, NodeSize node_size)
{
    // This is how much space is required.
    size_t st = size + MAJOR_BLOCK_HEADER_SIZE;
    st += MINOR_BLOCK_HEADER_SIZE;

    // Perfect amount of space?
    if ((st % MEM_PAGE_SIZE) == 0)
    {
        st = st / (MEM_PAGE_SIZE);
    }
    else
    {
        st = st / (MEM_PAGE_SIZE) + 1;
    }

    // Make sure it's >= the minimum size.
    st = m_max(st, (size_t)node_size / MEM_PAGE_SIZE); // The number of pages to request per chunk.

    HeapMajor *maj;
    if (embed_mem_acquire(st * MEM_PAGE_SIZE, (void **)&maj, EMBED_MEM_NONE).kind != ERR_KIND_SUCCESS)
    {
        panic$("Failed to allocate memory!");
    }

    maj->prev = nullptr;
    maj->next = nullptr;
    maj->pages = st;
    maj->size = st * MEM_PAGE_SIZE;
    maj->usage = MAJOR_BLOCK_HEADER_SIZE;
    maj->first = nullptr;

    return maj;
}

static bool check_minor_magic(HeapMinor *min, void *ptr)
{
    if (min->magic == ALLOC_HEAP_MAGIC)
    {
        return true;
    }

    // Check for overrun errors. For all bytes of ALLOC_HEAP_MAGIC
    if (((min->magic & 0xFFFFFF) == (ALLOC_HEAP_MAGIC & 0xFFFFFF)) ||
        ((min->magic & 0xFFFF) == (ALLOC_HEAP_MAGIC & 0xFFFF)) ||
        ((min->magic & 0xFF) == (ALLOC_HEAP_MAGIC & 0xFF)))
    {
        panic$("Possible 1-3 byte overrun for magic {08x} != {08x}.", min->magic, ALLOC_HEAP_MAGIC);
    }

    if (min->magic == ALLOC_HEAP_DEAD)
    {
        panic$("Multiple free({p}).", ptr);
    }
    else
    {
        panic$("Bad free({p}) magic is {08x}", ptr, min->magic);
    }

    return false;
}

void *heap_alloc_acquire(HeapAlloc *alloc, size_t req_size)
{
    req_size = align_up$(req_size, ALLOC_HEAP_ALIGN);

    size_t bestSize = 0;
    size_t size = req_size;

    if (size == 0)
    {
        log$("alloc(0) called");
        return heap_alloc_acquire(alloc, 1);
    }

    // Is this the first time we are being used?
    if (alloc->root == nullptr)
    {
        alloc->root = major_block_create(size, alloc->size);

        if (alloc->root == nullptr)
        {
            return nullptr;
        }
    }

    // Now we need to bounce through every major and find enough space....
    HeapMajor *maj = alloc->root;
    bool started_with_bestbet = false;

    // Start at the best bet....
    if (alloc->best != nullptr)
    {
        bestSize = alloc->best->size - alloc->best->usage;

        if (bestSize > (size + MINOR_BLOCK_HEADER_SIZE))
        {
            maj = alloc->best;
            started_with_bestbet = true;
        }
    }

    while (maj != nullptr)
    {
        size_t maj_diff = maj->size - maj->usage;
        // free memory in the block

        if (bestSize < maj_diff)
        {
            // Hmm.. this one has more memory then our bestBet. Remember!
            alloc->best = maj;
            bestSize = maj_diff;
        }

        // CASE 1:  There is not enough space in this major block.
        if (maj_diff < (size + MINOR_BLOCK_HEADER_SIZE))
        {

            // Another major block next to this one?
            if (maj->next != nullptr)
            {
                maj = maj->next; // Hop to that one.
                continue;
            }

            if (started_with_bestbet)
            {
                // let's start all over again.
                maj = alloc->root;
                started_with_bestbet = false;
                continue;
            }

            // Create a new major block next to this one and...
            maj->next = major_block_create(size, alloc->size);

            if (maj->next == nullptr)
            {
                break; // no more memory :sad:
            }

            maj->next->prev = maj;
            maj = maj->next;

            // .. fall through to CASE 2 ..
        }

        // CASE 2: It's a brand new block.
        if (maj->first == nullptr)
        {
            HeapMinor *min = (HeapMinor *)((uintptr_t)maj + MAJOR_BLOCK_HEADER_SIZE);

            min->magic = ALLOC_HEAP_MAGIC;
            min->prev = nullptr;
            min->next = nullptr;
            min->block = maj;
            min->size = size;
            min->req_size = req_size;

            maj->first = min;
            maj->usage += size + MINOR_BLOCK_HEADER_SIZE;

            return (void *)((uintptr_t)(maj->first) + MINOR_BLOCK_HEADER_SIZE);
        }

        {
            // CASE 3: Block in use and enough space at the start of the block.
            size_t diff = (uintptr_t)(maj->first);
            diff -= (uintptr_t)maj;
            diff -= MAJOR_BLOCK_HEADER_SIZE;

            if (diff >= (size + MINOR_BLOCK_HEADER_SIZE))
            {
                // Yes, space in front. Squeeze in.
                maj->first->prev = (HeapMinor *)((uintptr_t)maj + MAJOR_BLOCK_HEADER_SIZE);
                maj->first->prev->next = maj->first;
                maj->first = maj->first->prev;
                maj->first->magic = ALLOC_HEAP_MAGIC;
                maj->first->prev = nullptr;
                maj->first->block = maj;
                maj->first->size = size;
                maj->first->req_size = req_size;
                maj->usage += size + MINOR_BLOCK_HEADER_SIZE;

                return (void *)((uintptr_t)(maj->first) + MINOR_BLOCK_HEADER_SIZE);
            }
        }

        // CASE 4: There is enough space in this block. But is it contiguous?
        HeapMinor *min = maj->first;

        // Looping within the block now...
        while (min != nullptr)
        {
            // CASE 4.1: End of minors in a block. Space from last and end?
            if (min->next == nullptr)
            {
                // the rest of this block is free...  is it big enough?
                size_t diff = (uintptr_t)(maj) + maj->size;
                diff -= (uintptr_t)min;
                diff -= MINOR_BLOCK_HEADER_SIZE;
                diff -= min->size;
                // minus already existing usage..

                if (diff >= (size + MINOR_BLOCK_HEADER_SIZE))
                {
                    min->next = (HeapMinor *)((uintptr_t)min + MINOR_BLOCK_HEADER_SIZE + min->size);
                    min->next->prev = min;
                    min = min->next;
                    min->next = nullptr;
                    min->magic = ALLOC_HEAP_MAGIC;
                    min->block = maj;
                    min->size = size;
                    min->req_size = req_size;
                    maj->usage += size + MINOR_BLOCK_HEADER_SIZE;

                    return (void *)((uintptr_t)min + MINOR_BLOCK_HEADER_SIZE);
                }
            }

            // CASE 4.2: Is there space between two minors?
            if (min->next != nullptr)
            {
                // is the difference between here and next big enough?
                size_t diff = (uintptr_t)(min->next);
                diff -= (uintptr_t)min;
                diff -= MINOR_BLOCK_HEADER_SIZE;
                diff -= min->size;
                // minus our existing usage.

                if (diff >= (size + MINOR_BLOCK_HEADER_SIZE))
                {
                    HeapMinor *new_min = (HeapMinor *)((uintptr_t)min + MINOR_BLOCK_HEADER_SIZE + min->size);

                    new_min->magic = ALLOC_HEAP_MAGIC;
                    new_min->next = min->next;
                    new_min->prev = min;
                    new_min->size = size;
                    new_min->req_size = req_size;
                    new_min->block = maj;
                    min->next->prev = new_min;
                    min->next = new_min;
                    maj->usage += size + MINOR_BLOCK_HEADER_SIZE;

                    return (void *)((uintptr_t)new_min + MINOR_BLOCK_HEADER_SIZE);
                }
            } // min->next != nullptr

            min = min->next;
        } // while min != nullptr ...

        // CASE 5: Block full! Ensure next block and loop.
        if (maj->next == nullptr)
        {

            if (started_with_bestbet)
            {
                // let's start all over again.
                maj = alloc->root;
                started_with_bestbet = false;
                continue;
            }

            // we've run out. we need more...
            // next one guaranteed to be okay
            maj->next = major_block_create(size, alloc->size);

            if (maj->next == nullptr)
            {
                //  uh oh,  no more memory.....
                break;
            }

            maj->next->prev = maj;
        }

        maj = maj->next;
    } // while (maj != nullptr)

    log$("All cases exhausted. No memory available.");

    return nullptr;
}

void heap_alloc_release(HeapAlloc *alloc, void *ptr)
{
    if (ptr == nullptr)
    {
        log$("free( nullptr )");
        return;
    }

    HeapMinor *min = (HeapMinor *)((uintptr_t)ptr - MINOR_BLOCK_HEADER_SIZE);

    if (!check_minor_magic(min, ptr))
    {
        return;
    }

    HeapMajor *maj = min->block;
    maj->usage -= (min->size + MINOR_BLOCK_HEADER_SIZE);
    min->magic = ALLOC_HEAP_DEAD;

    if (min->next != nullptr)
    {
        min->next->prev = min->prev;
    }

    if (min->prev != nullptr)
    {
        min->prev->next = min->next;
    }

    if (min->prev == nullptr)
    {
        maj->first = min->next;
    }

    if (maj->first == nullptr)
    {
        if (alloc->root == maj)
        {
            alloc->root = maj->next;
        }

        if (alloc->best == maj)
        {
            alloc->best = nullptr;
        }

        if (maj->prev != nullptr)
        {
            maj->prev->next = maj->next;
        }

        if (maj->next != nullptr)
        {
            maj->next->prev = maj->prev;
        }

        embed_mem_release(maj, maj->pages * MEM_PAGE_SIZE);
    }
    else
    {
        if (alloc->best != nullptr)
        {
            int bestSize = alloc->best->size - alloc->best->usage;
            int majSize = maj->size - maj->usage;

            if (majSize > bestSize)
            {
                alloc->best = maj;
            }
        }
    }
}

void *heap_alloc_resize(HeapAlloc *alloc, void *ptr, size_t size)
{
    size = align_up$(size, ALLOC_HEAP_ALIGN);

    if (size == 0)
    {
        heap_alloc_release(alloc, ptr);
        return nullptr;
    }

    if (ptr == nullptr)
    {
        return heap_alloc_acquire(alloc, size);
    }

    HeapMinor *min = (HeapMinor *)((uintptr_t)ptr - MINOR_BLOCK_HEADER_SIZE);

    if (!check_minor_magic(min, ptr))
    {
        return nullptr;
    }

    if (min->size >= size)
    {
        min->req_size = size;
        return ptr;
    }

    void *new_ptr = heap_alloc_acquire(alloc, size);
    mem_cpy(new_ptr, ptr, min->req_size);
    heap_alloc_release(alloc, ptr);

    return new_ptr;
}

void heap_alloc_init(HeapAlloc *alloc, NodeSize size)
{
    *alloc = (HeapAlloc){
        .base = {
            .acquire = (AllocAcquire *)heap_alloc_acquire,
            .resize = (AllocResize *)heap_alloc_resize,
            .release = (AllocRelease *)heap_alloc_release,
        },
        .size = size,
        .best = nullptr,
        .root = nullptr,
    };
}

void heap_alloc_deinit(HeapAlloc *alloc)
{
    HeapMajor *current = alloc->root;
    while (current)
    {
        HeapMajor *next = current->next;
        embed_mem_release(current, current->pages * MEM_PAGE_SIZE);
        current = next;
    }

    *alloc = (HeapAlloc){};
}
