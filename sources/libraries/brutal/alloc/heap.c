#include <brutal/alloc/heap.h>
#include <brutal/host/mem.h>
#include <brutal/log.h>

#define ALLOC_HEAP_ALIGN (64)
#define ALLOC_HEAP_REQUEST (16)
#define ALLOC_HEAP_MAGIC (0xc001c0de)
#define ALLOC_HEAP_DEAD (0xdeaddead)

struct alloc_minor;

struct alloc_major
{
    size_t pages;

    size_t size;
    size_t usage;

    struct alloc_major *prev;
    struct alloc_major *next;

    struct alloc_minor *first;
};

#define MAJOR_BLOCK_HEADER_SIZE (ALIGN_UP(sizeof(struct alloc_major), ALLOC_HEAP_ALIGN))

struct alloc_minor
{
    size_t magic;

    size_t size;
    size_t req_size;

    struct alloc_minor *prev;
    struct alloc_minor *next;

    struct alloc_major *block;
};

#define MINOR_BLOCK_HEADER_SIZE (ALIGN_UP(sizeof(struct alloc_minor), ALLOC_HEAP_ALIGN))

static struct alloc_major *major_block_create(size_t size)
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
    st = MAX(st, ALLOC_HEAP_REQUEST); // The number of pages to request per chunk.

    struct alloc_major *maj;
    if (host_mem_acquire(st * MEM_PAGE_SIZE, (void **)&maj, HOST_MEM_NONE).kind != ERR_KIND_SUCCESS)
    {
        panic("Failled to allocate memory!");
    }

    maj->prev = nullptr;
    maj->next = nullptr;
    maj->pages = st;
    maj->size = st * MEM_PAGE_SIZE;
    maj->usage = MAJOR_BLOCK_HEADER_SIZE;
    maj->first = nullptr;

    return maj;
}

static bool check_minor_magic(struct alloc_minor *min, void *ptr)
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
        panic("Possible 1-3 byte overrun for magic {08x} != {08x}.", min->magic, ALLOC_HEAP_MAGIC);
    }

    if (min->magic == ALLOC_HEAP_DEAD)
    {
        panic("Multiple free({p}).", ptr);
    }
    else
    {
        panic("Bad free({p})", ptr);
    }

    return false;
}

void *alloc_heap_acquire(struct alloc_heap *alloc, size_t req_size)
{
    req_size = ALIGN_UP(req_size, ALLOC_HEAP_ALIGN);

    unsigned long long bestSize = 0;
    unsigned long size = req_size;

    if (size == 0)
    {
        log("alloc(0) called");
        return alloc_heap_acquire(alloc, 1);
    }

    // Is this the first time we are being used?
    if (alloc->root == nullptr)
    {
        alloc->root = major_block_create(size);

        if (alloc->root == nullptr)
        {
            return nullptr;
        }
    }

    // Now we need to bounce through every major and find enough space....
    struct alloc_major *maj = alloc->root;
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
        size_t diff = maj->size - maj->usage;
        // free memory in the block

        if (bestSize < diff)
        {
            // Hmm.. this one has more memory then our bestBet. Remember!
            alloc->best = maj;
            bestSize = diff;
        }

        // CASE 1:  There is not enough space in this major block.
        if (diff < (size + MINOR_BLOCK_HEADER_SIZE))
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
            maj->next = major_block_create(size);

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
            maj->first = (struct alloc_minor *)((uintptr_t)maj + MAJOR_BLOCK_HEADER_SIZE);

            maj->first->magic = ALLOC_HEAP_MAGIC;
            maj->first->prev = nullptr;
            maj->first->next = nullptr;
            maj->first->block = maj;
            maj->first->size = size;
            maj->first->req_size = req_size;
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
                maj->first->prev = (struct alloc_minor *)((uintptr_t)maj + MAJOR_BLOCK_HEADER_SIZE);
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
        struct alloc_minor *min = maj->first;

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
                    min->next = (struct alloc_minor *)((uintptr_t)min + MINOR_BLOCK_HEADER_SIZE + min->size);
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
                    struct alloc_minor *new_min = (struct alloc_minor *)((uintptr_t)min + MINOR_BLOCK_HEADER_SIZE + min->size);

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
            maj->next = major_block_create(size);

            if (maj->next == nullptr)
            {
                //  uh oh,  no more memory.....
                break;
            }

            maj->next->prev = maj;
        }

        maj = maj->next;
    } // while (maj != nullptr)

    log("All cases exhausted. No memory available.");

    return nullptr;
}

void alloc_heap_release(struct alloc_heap *alloc, void *ptr)
{
    if (ptr == nullptr)
    {
        log("free( nullptr )");
        return;
    }

    struct alloc_minor *min = (struct alloc_minor *)((uintptr_t)ptr - MINOR_BLOCK_HEADER_SIZE);

    if (!check_minor_magic(min, ptr))
    {
        return;
    }

    struct alloc_major *maj = min->block;

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

        host_mem_release(maj, maj->pages * MEM_PAGE_SIZE);
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

void *alloc_heap_resize(struct alloc_heap *alloc, void *ptr, size_t size)
{
    size = ALIGN_UP(size, ALLOC_HEAP_ALIGN);

    if (size == 0)
    {
        alloc_heap_release(alloc, ptr);
        return nullptr;
    }

    if (ptr == nullptr)
    {
        return alloc_heap_acquire(alloc, size);
    }

    auto min = (struct alloc_minor *)((uintptr_t)ptr - MINOR_BLOCK_HEADER_SIZE);

    if (!check_minor_magic(min, ptr))
    {
        return nullptr;
    }

    if (min->size >= size)
    {
        min->req_size = size;
        return ptr;
    }

    void *new_ptr = alloc_heap_acquire(alloc, size);
    mem_cpy(new_ptr, ptr, min->req_size);
    alloc_heap_release(alloc, ptr);

    return new_ptr;
}

void alloc_heap_init(struct alloc_heap *alloc)
{
    *alloc = (struct alloc_heap){
        .base = {
            .acquire = (AllocAcquire *)alloc_heap_acquire,
            .resize = (AllocResize *)alloc_heap_resize,
            .release = (AllocRelease *)alloc_heap_release,
        },
        .best = nullptr,
        .root = nullptr,
    };
}
