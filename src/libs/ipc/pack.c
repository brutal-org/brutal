#include <ipc/pack.h>
#include <brutal-debug>
#include <brutal-mem>

void ipc_pack_init(IpcPack *self)
{
    *self = (IpcPack){};
}

void ipc_pack_deinit(IpcPack *self)
{
    for (int i = 0; i < self->handles_count; i++)
    {
        bal_close(self->handles[i]);
    }

    if (self->buf != nullptr)
    {
        assert_br_success(bal_unmap(BR_HANDLE_SELF, self->buf, self->len));
        assert_br_success(bal_close(self->handle));
    }
}

void ipc_pack_ensure(IpcPack *self, size_t cap)
{
    if (self->len >= cap)
    {
        return;
    }

    BrCreateArgs memory = {
        .type = BR_OBJECT_MEMORY,
        .memory = {
            .size = align_up$(cap, MEM_PAGE_SIZE),
        },
    };

    assert_br_success(br_create(&memory));

    BrMapArgs memmap = {
        .space = BR_HANDLE_SELF,
        .memory = memory.handle,
        .flags = BR_MEM_READABLE | BR_MEM_WRITABLE,
    };

    assert_br_success(br_map(&memmap));

    if (self->buf != nullptr)
    {
        mem_cpy((void *)memmap.vaddr, self->buf, self->curr);
        assert_br_success(bal_unmap(BR_HANDLE_SELF, self->buf, self->len));
        assert_br_success(bal_close(self->handle));
    }

    self->handle = memory.handle;
    self->buf = (void *)memmap.vaddr;
    self->len = align_up$(cap, MEM_PAGE_SIZE);
}

void ipc_pack(IpcPack *self, void const *buf, size_t len)
{
    ipc_pack_ensure(self, self->curr + len);

    mem_cpy(self->buf + self->curr, buf, len);
    self->curr += len;
}

void ipc_pack_handle(IpcPack *self, BrHandle const *handle)
{
    assert_lower_than(self->handles_count, BR_MSG_ARG_COUNT - 1);
    self->handles[self->handles_count++] = *handle;
    ipc_pack_pod(self, handle);
}

void ipc_pack_str(IpcPack *self, Str const *v)
{
    ipc_pack_pod(self, &v->len);
    ipc_pack(self, v->buf, v->len);
}

void ipc_pack_slice_impl(IpcPack *self, SliceImpl const *v, IpcPackFn *el)
{
    ipc_pack_pod(self, &v->len);

    for (size_t i = 0; i < v->len; i++)
    {
        el(self, v->buf + v->size * i);
    }
}
