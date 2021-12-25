#include <bal/hw/mem.h>
#include <brutal/base.h>
#include <brutal/debug.h>

static BrResult bal_mem_map(BalMem *self, size_t size, size_t offset)
{
    assert_equal(self->buf, nullptr);

    BrMapArgs args = {
        .space = BR_HANDLE_SELF,
        .memory = self->handle,
        .size = size,
        .offset = offset,
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
        .space = BR_HANDLE_SELF,
        .size = self->len,
        .vaddr = (uintptr_t)self->buf,
    };

    self->buf = nullptr;

    return br_unmap(&args);
}

MaybeError bal_mem_init_mobj(BalMem *self, BrHandle handle)
{
    return bal_mem_init_mobj_offset(self, handle, 0, 0);
}

MaybeError bal_mem_init_mobj_offset(BalMem *self, BrHandle handle, size_t offset, size_t len)
{
    *self = (BalMem){};

    self->handle = handle;

    BrResult result = bal_mem_map(self, len, offset);

    if (result != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(result));
    }

    return SUCCESS;
}

MaybeError bal_mem_init_pmm(BalMem *self, uintptr_t addr, size_t size)
{
    BrCreateArgs memory = {
        .type = BR_OBJECT_MEMORY,
        .memory = {
            .addr = addr,
            .size = size,
            .flags = BR_MEM_PMM,
        },
    };

    BrResult result = br_create(&memory);

    self->paddr = memory.memory.addr;

    if (result != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(result));
    }

    TRY(MaybeError, bal_mem_init_mobj(self, memory.handle));

    self->paddr = memory.memory.addr;
    return SUCCESS;
}

MaybeError bal_mem_init_size(BalMem *self, size_t size)
{
    BrCreateArgs memory_args = {
        .type = BR_OBJECT_MEMORY,
        .memory = {
            .size = ALIGN_UP(size, 4096),
            .flags = 0,
        },
    };

    BrResult result = br_create(&memory_args);

    if (result != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(result));
    }

    TRY(MaybeError, bal_mem_init_mobj(self, memory_args.handle));

    self->paddr = memory_args.memory.addr;
    return SUCCESS;
}

MaybeError bal_mem_deinit(BalMem *self)
{
    bal_mem_unmap(self);

    BrResult result = bal_close(self->handle);

    if (result != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(result));
    }

    return SUCCESS;
}

MaybeError bal_memobj_paddr(BrMemObj obj, uintptr_t *paddr)
{
    BrStatArgs args = {
        .handle = obj,
    };

    BrResult res = br_stat(&args);

    if (res != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(res));
    }

    *paddr = args.info.memobj.range.base;

    return SUCCESS;
}
