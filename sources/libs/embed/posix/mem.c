#include <brutal-debug>
#include <embed/mem.h>
#include <embed/posix/err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

void embed_mem_lock(void)
{
    // FIXME: no-op
}

void embed_mem_unlock(void)
{
    // FIXME: no-op
}

Error embed_mem_acquire(size_t size, void **out_result, MAYBE_UNUSED enum embed_mem_flag flags)
{
    *out_result = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (*out_result == MAP_FAILED)
    {
        return error_from_errno();
    }

    return ERR_SUCCESS;
}

Error embed_mem_release(void *addr, size_t size)
{
    if (munmap(addr, size) == -1)
    {
        return error_from_errno();
    }

    return ERR_SUCCESS;
}

void embed_heap_lock(void)
{
    // no-op
}

void embed_heap_unlock(void)
{
    // no-op
}

void *embed_heap_acquire(size_t size)
{
    return calloc(1, size);
}

void *embed_heap_resize(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void embed_heap_release(void *ptr)
{
    free(ptr);
}
