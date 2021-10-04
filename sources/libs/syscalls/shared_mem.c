#include <brutal/base.h>
#include <brutal/log.h>
#include <syscalls/shared_mem.h>
BrResult shared_mem_map(SharedMem *self)
{
    if (self->data != nullptr)
    {
        return BR_SUCCESS;
    }
    BrMapArgs map_args = {
        .space = BR_SPACE_SELF,
        .mem_obj = self->obj,
        .flags = BR_MEM_WRITABLE,

    };

    BrResult r = br_map(&map_args);

    if (r != BR_SUCCESS)
    {
        return r;
    }

    self->map = map_args;
    self->data = (void *)self->map.vaddr;
    self->size = map_args.size;
    return BR_SUCCESS;
}

BrResult shared_mem_unmap(SharedMem *self)
{
    if (self->data == nullptr)
    {
        return BR_BAD_ARGUMENTS;
    }

    BrUnmapArgs unmap_args =
        {
            .space = BR_SPACE_SELF,
            .size = self->map.size,
            .vaddr = self->map.vaddr,
        };

    self->data = nullptr;
    return br_unmap(&unmap_args);
}

SharedMemResult shared_mem_alloc(size_t size)
{
    SharedMem result = {};

    BrCreateArgs mem_obj_args = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .size = ALIGN_UP(size, 4096),
            .flags = 0,
        },
    };

    TRY_V(SharedMemResult, br_create(&mem_obj_args), BR_SUCCESS);

    result.obj = mem_obj_args.handle;

    TRY_V(SharedMemResult, shared_mem_map(&result), BR_SUCCESS);

    return OK(SharedMemResult, result);
}

BrResult shared_mem_free(SharedMem *self)
{
    shared_mem_unmap(self);
    BrCloseArgs close_args = {
        .handle = self->obj,
    };

    return br_close(&close_args);
}

SharedMem memobj_to_shared(BrMemObj obj)
{
    SharedMem mem = {0};

    mem.obj = obj;
    return mem;
}

BrMemObj shared_to_memobj(SharedMem from)
{
    return from.obj;
}

Str shared_mem_to_str(SharedMem *self)
{
    InlineStr *r = self->data;
    return str$(r);
}

SharedMemResult shared_mem_from_str(Str str)
{
    SharedMem mem = TRY(SharedMemResult, shared_mem_alloc(str.len + sizeof(InlineStr) + 1));

    InlineStr *target = mem.data;

    mem_cpy(target->buffer, str.buffer, str.len);
    target->len = str.len;

    return OK(SharedMemResult, mem);
}
