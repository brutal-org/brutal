#include <brutal/log.h>
#include <syscalls/shared_mem.h>

void *shared_mem_map(SharedMem *self)
{
    if (self->data != nullptr)
    {
        return self->data->data;
    }

    BrMapArgs map_args = {
        .space = BR_SPACE_SELF,
        .mem_obj = self->obj,
        .flags = BR_MEM_WRITABLE | BR_MEM_USER,

    };

    assert_br_success(br_map(&map_args));

    self->map = map_args;
    self->data = (SharedMemData *)self->map.vaddr;

    return self->data->data;
}

void shared_mem_unmap(SharedMem *self)
{
    if (self->data == nullptr)
    {
        return;
    }

    BrUnmapArgs unmap_args =
        {
            .space = BR_SPACE_SELF,
            .size = self->map.size,
            .vaddr = self->map.vaddr,
        };

    assert_br_success(br_unmap(&unmap_args));
}

SharedMem shared_mem_alloc(size_t size)
{
    SharedMem result = {};

    BrCreateArgs mem_obj_args = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .addr = 0,
            .size = ALIGN_UP(size + sizeof(SharedMemData), 4096),
            .flags = BR_MEM_OBJ_PMM,
        },
    };

    assert_br_success(br_create(&mem_obj_args));

    result.obj = mem_obj_args.handle;

    shared_mem_map(&result);

    result.data->len = size;
    return result;
}

void shared_mem_free(SharedMem *self)
{
    BrUnmapArgs unmap_args = {
        .space = BR_SPACE_SELF,
        .size = self->map.size,
        .vaddr = self->map.vaddr,
    };

    br_unmap(&unmap_args);
    self->data = nullptr;
}

void shared_mem_destroy(SharedMem *self)
{
    BrCloseArgs close_args = {
        .handle = self->obj,
    };

    br_close(&close_args);
}
SharedMem memobj_to_shared(BrMemObj obj)
{
    SharedMem mem = {
        .obj = obj,
    };

    return mem;
}

BrMemObj shared_to_memobj(SharedMem from)
{
    return from.obj;
}
