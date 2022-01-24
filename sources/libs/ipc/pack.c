#include <brutal/debug.h>
#include <brutal/mem.h>
#include <ipc/pack.h>

void ipc_pack_init(IpcPack *self)
{
    *self = (IpcPack){};
}

void ipc_pack_deinit(IpcPack *self)
{
    assert_br_success(bal_unmap(BR_HANDLE_SELF, self->buf, self->len));
    assert_br_success(bal_close(self->handle));
}

void ipc_pack_ensure(IpcPack *self, size_t cap)
{
    if (self->len > cap)
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
        bal_unmap(BR_HANDLE_SELF, self->buf, self->len);
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

void ipc_pack_enum(IpcPack *self, int const *v)
{
    ipc_pack(self, v, sizeof(*v));
}

void ipc_pack_size(IpcPack *self, size_t const *s)
{
    ipc_pack(self, s, sizeof(*s));
}

void ipc_pack_ptr(IpcPack *self, uintptr_t const *s)
{
    ipc_pack(self, s, sizeof(*s));
}

void ipc_pack_s8(IpcPack *self, int8_t const *v)
{
    ipc_pack(self, v, sizeof(*v));
}

void ipc_pack_u8(IpcPack *self, uint8_t const *v)
{
    ipc_pack(self, v, sizeof(*v));
}

void ipc_pack_s16(IpcPack *self, int16_t const *v)
{
    ipc_pack(self, v, sizeof(*v));
}

void ipc_pack_u16(IpcPack *self, uint16_t const *v)
{
    ipc_pack(self, v, sizeof(*v));
}

void ipc_pack_s32(IpcPack *self, int32_t const *v)
{
    ipc_pack(self, v, sizeof(*v));
}

void ipc_pack_u32(IpcPack *self, uint32_t const *v)
{
    ipc_pack(self, v, sizeof(*v));
}

void ipc_pack_s64(IpcPack *self, int64_t const *v)
{
    ipc_pack(self, v, sizeof(*v));
}

void ipc_pack_u64(IpcPack *self, uint64_t const *v)
{
    ipc_pack(self, v, sizeof(*v));
}

void ipc_pack_f32(IpcPack *self, float const *v)
{
    ipc_pack(self, v, sizeof(*v));
}

void ipc_pack_f64(IpcPack *self, double const *v)
{
    ipc_pack(self, v, sizeof(*v));
}

void ipc_pack_str(IpcPack *self, Str const *v)
{
    ipc_pack_size(self, &v->len);
    ipc_pack(self, v->buf, v->len);
}

void ipc_pack_cap(IpcPack *self, IpcCap const *cap)
{
    ipc_pack(self, cap, sizeof(*cap));
}

void ipc_pack_proto(IpcPack *self, IpcProto const *v)
{
    ipc_pack(self, v, sizeof(*v));
}

void ipc_pack_slice_impl(IpcPack *self, SliceImpl const *v, IpcPackFn *el)
{
    ipc_pack_size(self, &v->len);

    for (size_t i = 0; i < v->len; i++)
    {
        el(self, v->buf + v->size * i);
    }
}
