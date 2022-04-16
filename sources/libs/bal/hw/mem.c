#include <bal/hw/mem.h>
#include <brutal-base>
#include <brutal-debug>

MaybeError bal_mem_init_mobj(BalMem *self, BrHandle handle)
{
    return bal_mem_init_mobj_offset(self, handle, 0, 0);
}

MaybeError bal_mem_init_mobj_offset(BalMem *self, BrHandle handle, size_t offset, size_t len)
{
    *self = (BalMem){
        .off = offset,
        .len = len,
        .handle = handle,
    };

    return SUCCESS;
}

MaybeError bal_mem_init_pmm(BalMem *self, uintptr_t addr, size_t size)
{
    BrCreateArgs memory = {
        .type = BR_OBJECT_MEMORY,
        .memory = {
            .addr = addr,
            .size = size,
            .flags = BR_MEM_DMA,
        },
    };

    BrResult result = br_create(&memory);

    if (result != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(result));
    }

    TRY(MaybeError, bal_mem_init_mobj(self, memory.handle));

    return SUCCESS;
}

MaybeError bal_mem_init(BalMem *self, size_t size)
{
    BrCreateArgs memory_args = {
        .type = BR_OBJECT_MEMORY,
        .memory = {
            .size = align_up$(size, MEM_PAGE_SIZE),
            .flags = 0,
        },
    };

    BrResult result = br_create(&memory_args);

    if (result != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(result));
    }

    TRY(MaybeError, bal_mem_init_mobj(self, memory_args.handle));

    return SUCCESS;
}

MaybeError bal_mem_deinit(BalMem *self)
{
    if (self->buf)
    {
        bal_mem_unmap(self);
    }

    BrResult result = bal_close(self->handle);

    if (result != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(result));
    }

    return SUCCESS;
}

void bal_mem_dup(BalMem *dst, BalMem *src)
{
    bal_mem_init_mobj(dst, bal_dup(src->handle));
}

BrResult bal_mem_map(BalMem *self)
{
    assert_equal(self->buf, nullptr);

    BrMapArgs args = {
        .space = BR_HANDLE_SELF,
        .memory = self->handle,
        .size = self->len,
        .offset = self->off,
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

BrResult bal_mem_unmap(BalMem *self)
{
    if (!self->buf)
    {
        return BR_SUCCESS;
    }

    BrUnmapArgs args = {
        .space = BR_HANDLE_SELF,
        .size = self->len,
        .vaddr = (uintptr_t)self->buf,
    };

    self->buf = nullptr;

    return br_unmap(&args);
}

MaybeError bal_mem_paddr(BalMem *self, uintptr_t *paddr)
{
    BrInspectArgs args = {
        .handle = self->handle,
    };

    BrResult res = br_inspect(&args);

    if (res != BR_SUCCESS)
    {
        return ERR(MaybeError, br_result_to_error(res));
    }

    *paddr = args.memory.range.base;

    return SUCCESS;
}

void bal_mem_pack(IpcPack *pack, BalMem const *self)
{
    ipc_pack_handle(pack, &self->handle);
}

void bal_mem_unpack(IpcUnpack *pack, BalMem *self)
{
    *self = (BalMem){};
    ipc_unpack_handle(pack, &self->handle);
}
