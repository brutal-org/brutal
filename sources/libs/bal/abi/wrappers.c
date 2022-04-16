#include <bal/abi/syscalls.h>
#include <bal/abi/wrappers.h>
#include <brutal-debug>

BrResult bal_unmap(BrHandle space, void *base, size_t len)
{
    return br_unmap(&(BrUnmapArgs){
        space,
        (uintptr_t)base,
        len,
    });
}

BrHandle bal_dup(BrHandle handle)
{
    BrDupArgs dup = {
        .handle = handle,
    };

    br_dup(&dup);

    return dup.copy;
}

BrResult bal_close(BrHandle handle)
{
    return br_close(&(BrCloseArgs){
        .handle = handle,
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

BrResult bal_ack(BrEvent event)
{
    return br_ack(&(BrAckArgs){
        .event = event,
    });
}
