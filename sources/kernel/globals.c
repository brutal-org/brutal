#include <brutal/debug.h>
#include <brutal/mem.h>
#include <brutal/sync.h>
#include "kernel/globals.h"

static Lock _lock;
static HeapRange _heap;
static BrGlobals *_info = nullptr;

static void global_ensure(void)
{
    LOCK_RETAINER(&_lock);

    if (_info == nullptr)
    {
        _heap = UNWRAP(heap_alloc(ALIGN_UP(sizeof(*_info), MEM_PAGE_SIZE)));
        _info = (BrGlobals *)range_begin(_heap);

        *_info = (BrGlobals){};
    }
}

BrGlobals *global(void)
{
    global_ensure();
    return _info;
}

HeapRange global_region(void)
{
    global_ensure();
    return _heap;
}

MemObj *global_create_obj(void)
{
    global_ensure();
    return mem_obj_heap(_heap, MEM_OBJ_NONE);
}
