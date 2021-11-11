#include <bal/helpers.h>
#include <bal/shm.h>
#include <brutal/base.h>
#include <brutal/debug.h>

static BrResult balshm_map(BalShm *self)
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

static BrResult balshm_unmap(BalShm *self)
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

MaybeError balshm_init_mobj(BalShm *self, BrMemObj obj)
{
    self->obj = obj;
    BrResult result = balshm_map(self);

    if (result != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(result));
    }

    return SUCCESS;
}

MaybeError balshm_init_size(BalShm *self, size_t size)
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

    return balshm_init_mobj(self, mem_obj_args.handle);
}

MaybeError balshm_deinit(BalShm *self)
{
    balshm_unmap(self);

    BrResult result = brh_close(self->obj);

    if (result != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(result));
    }

    return SUCCESS;
}
