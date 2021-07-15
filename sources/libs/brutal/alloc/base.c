#include <brutal/alloc/base.h>
#include <brutal/base/attributes.h>

void alloc_no_op(MAYBE_UNUSED Alloc *self, MAYBE_UNUSED void *ptr)
{
}

void *alloc_malloc(Alloc *self, size_t size)
{
    void *ptr = alloc_acquire(self, size);
    alloc_commit(self, ptr);
    return ptr;
}

void alloc_free(Alloc *self, void *ptr)
{
    alloc_decommit(self, ptr);
    alloc_release(self, ptr);
}

void *alloc_calloc(Alloc *self, size_t size, size_t count)
{
    void *ptr = alloc_acquire(self, size * count);
    alloc_commit(self, ptr);
    mem_set(ptr, 0, size * count);
    return ptr;
}
