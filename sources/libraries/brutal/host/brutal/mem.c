#include <brutal/host/mem.h>
#include <syscalls/syscalls.h>

void host_mem_lock(void)
{
    // NO-OP
}

void host_mem_unlock(void)
{
    // NO-OP
}

Error host_mem_acquire(size_t size, void **out_result, MAYBE_UNUSED enum host_mem_flag flags)
{
    BrMObj obj;
    BrResult result;
    uintptr_t vaddr = 0;

    result = br_mobj(&obj, 0, size, BR_MOBJ_NONE);

    if (result != BR_SUCCESS)
    {
        return br_result_to_error(result);
    }

    result = br_alloc(BR_SPACE_SELF, obj, &vaddr, BR_MEM_WRITABLE);
    br_close(obj);

    if (result != BR_SUCCESS)
    {
        return br_result_to_error(result);
    }

    *out_result = (void *)vaddr;

    return ERR_SUCCESS;
}

Error host_mem_release(void *addr, size_t size)
{
    auto result = br_unmap(BR_SPACE_SELF, (uintptr_t)addr, size);
    return br_result_to_error(result);
}
