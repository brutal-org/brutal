#include <bal/abi/syscalls.h>
#include <bal/abi/wrappers.h>
#include <brutal/debug.h>

BrResult bal_close(BrHandle handle)
{
    return br_close(&(BrCloseArgs){
        .handle = handle,
    });
}

BrResult bal_unmap(BrHandle space, void *base, size_t len)
{
    return br_unmap(&(BrUnmapArgs){
        space,
        (uintptr_t)base,
        len,
    });
}

BrId bal_self_id(void)
{
    static BrId cache = BR_ID_NIL;
    if (cache == BR_ID_NIL)
    {
        BrInspectArgs inspect = {};
        inspect.handle = BR_HANDLE_SELF;
        br_inspect(&inspect);
        cache = inspect.id;
    }
    return cache;
}
