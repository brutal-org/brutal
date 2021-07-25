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

    BrCreateArgs mem_obj_args = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .size = size,
        },
    };

    BrResult result = br_create(&mem_obj_args);

    if (result != BR_SUCCESS)
    {
        return br_result_to_error(result);
    }

    BrMemObj mem_obj = mem_obj_args.mem_obj_handle;

    // Map the memory object

    BrMapArgs map_args = {
        .space = BR_SPACE_SELF,
        .mem_obj = mem_obj,
        .flags = BR_MEM_WRITABLE,
    };

    result = br_map(&map_args);

    // Cleanup and return

    br_close(&(BrCloseArgs){
        .handle = mem_obj,
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
