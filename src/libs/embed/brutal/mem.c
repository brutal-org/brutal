#include <bal/abi.h>
#include <brutal-alloc>
#include <brutal-sync>
#include <embed/mem.h>

Error embed_mem_acquire(size_t size, void **out_result, MAYBE_UNUSED enum embed_mem_flag flags)
{
    // Create the memory object

    BrCreateArgs memory = {
        .type = BR_OBJECT_MEMORY,
        .memory = {
            .size = size,
        },
    };

    BrResult result = br_create(&memory);

    if (result != BR_SUCCESS)
    {
        return br_result_to_error(result);
    }

    // Map the memory object

    BrMapArgs map_args = {
        .space = BR_HANDLE_SELF,
        .memory = memory.handle,
        .flags = BR_MEM_WRITABLE,
    };

    result = br_map(&map_args);

    // Cleanup and return

    bal_close(memory.handle);

    if (result != BR_SUCCESS)
    {
        return br_result_to_error(result);
    }

    *out_result = (void *)map_args.vaddr;

    return ERR_SUCCESS;
}

Error embed_mem_release(void *addr, size_t size)
{
    BrResult result = br_unmap(&(BrUnmapArgs){
        .space = BR_HANDLE_SELF,
        .vaddr = (uintptr_t)addr,
        .size = size,
    });

    return br_result_to_error(result);
}

static HeapAlloc _heap;

static Alloc *ensure_heap(void)
{
    static bool init = false;
    if (!init)
    {
        heap_alloc_init(&_heap, NODE_DEFAULT);
        init = true;
    }
    return (Alloc *)&_heap;
}

void embed_heap_lock(void)
{
}

void embed_heap_unlock(void)
{
}

void *embed_heap_acquire(size_t size)
{
    return alloc_acquire(ensure_heap(), size);
}

void *embed_heap_resize(void *ptr, size_t size)
{
    return alloc_resize(ensure_heap(), ptr, size);
}

void embed_heap_release(void *ptr)
{
    alloc_release(ensure_heap(), ptr);
}
