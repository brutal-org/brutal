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
    // Create the memory object

    BrMObjArgs mobj_args = {
        .size = size,
    };

    BrResult result = br_mobj(&mobj_args);

    if (result != BR_SUCCESS)
    {
        return br_result_to_error(result);
    }

    BrMObj mobj = mobj_args.mobj;

    // Map the memory object

    BrMapArgs map_args = {
        .space = BR_SPACE_SELF,
        .mobj = mobj,
        .flags = BR_MEM_WRITABLE,
    };

    result = br_map(&map_args);

    // Cleanup and return

    br_close(&(BrCloseArgs){
        .handle = mobj,
    });

    if (result != BR_SUCCESS)
    {
        return br_result_to_error(result);
    }

    *out_result = (void *)map_args.vaddr;

    return ERR_SUCCESS;
}

Error host_mem_release(void *addr, size_t size)
{
    auto result = br_unmap(&(BrUnmapArgs){
        .space = BR_SPACE_SELF,
        .vaddr = (uintptr_t)addr,
        .size = size,
    });

    return br_result_to_error(result);
}
