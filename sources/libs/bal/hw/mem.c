#include <bal/hw/mem.h>
#include <brutal/base.h>
#include <brutal/debug.h>

static BrResult bal_mem_map(BalMem *self)
{
    assert_equal(self->buf, nullptr);

    BrMapArgs args = {
        .space = BR_SPACE_SELF,
        .mem_obj = self->obj,
        .flags = BR_MEM_WRITABLE,
    };

    BrResult result = br_map(&args);

    if (result != BR_SUCCESS)
    {
        return result;
    }

    self->buf = (void *)args.vaddr;
    self->len = args.size;

    return BR_SUCCESS;
}

static BrResult bal_mem_unmap(BalMem *self)
{
    assert_not_equal(self->buf, nullptr);

    BrUnmapArgs args = {
        .space = BR_SPACE_SELF,
        .size = self->len,
        .vaddr = (uintptr_t)self->buf,
    };

    self->buf = nullptr;

    return br_unmap(&args);
}

MaybeError bal_mem_init_mobj(BalMem *self, BrMemObj obj)
{
    *self = (BalMem){};

    self->obj = obj;
    BrResult result = bal_mem_map(self);

    if (result != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(result));
    }

    return SUCCESS;
}

MaybeError bal_mem_init_pmm(BalMem *self, uintptr_t addr, size_t size)
{
    BrCreateArgs mem_obj = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .addr = addr,
            .size = size,
            .flags = BR_MEM_OBJ_PMM,
        },
    };

    BrResult result = br_create(&mem_obj);

    if (result != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(result));
    }

    self->obj = mem_obj.handle;

    result = bal_mem_map(self);

    if (result != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(result));
    }

    return SUCCESS;
}

MaybeError bal_mem_init_size(BalMem *self, size_t size)
{
    BrCreateArgs mem_obj_args = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .size = ALIGN_UP(size, 4096),
            .flags = 0,
        },
    };

    BrResult result = br_create(&mem_obj_args);

    if (result != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(result));
    }

    return bal_mem_init_mobj(self, mem_obj_args.handle);
}

MaybeError bal_mem_deinit(BalMem *self)
{
    bal_mem_unmap(self);

    BrResult result = bal_close(self->obj);

    if (result != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(result));
    }

    return SUCCESS;
}
