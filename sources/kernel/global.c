#include <brutal/log.h>
#include <brutal/mem.h>
#include <brutal/sync.h>
#include "kernel/global.h"

Lock lock;
HeapRange heap;
BrGlobalInfo *info = nullptr;

static void global_ensure(void)
{
    LOCK_RETAINER(&lock);

    if (info == nullptr)
    {
        heap = UNWRAP(heap_alloc(ALIGN_UP(sizeof(info), MEM_PAGE_SIZE)));
        info = (BrGlobalInfo *)range_begin(heap);

        *info = (BrGlobalInfo){};
    }
}

BrGlobalInfo *global(void)
{
    global_ensure();
    return info;
}

HeapRange global_region(void)
{
    global_ensure();
    return heap;
}

MemObj *global_create_obj(void)
{
    global_ensure();
    return mem_obj_heap(heap, MEM_OBJ_NONE);
}
