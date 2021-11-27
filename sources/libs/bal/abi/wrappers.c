#include <bal/abi/syscalls.h>
#include <bal/abi/wrappers.h>
#include <brutal/debug.h>

BrResult bal_close(BrHandle handle)
{
    return br_close(&(BrCloseArgs){
        .handle = handle,
    });
}

BrResult bal_unmap(BrSpace space, void *base, size_t len)
{
    return br_unmap(&(BrUnmapArgs){
        space,
        (uintptr_t)base,
        len,
    });
}
