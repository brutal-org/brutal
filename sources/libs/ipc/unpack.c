#include <ipc/unpack.h>

void ipc_unpack_init(IpcUnpack *self, void *buf, size_t len, Alloc *alloc)
{
    self->buf = buf;
    self->len = len;
    self->curr = 0;
    self->alloc = alloc;
}

void ipc_unpack(IpcUnpack *self, void *buf, size_t len)
{
    mem_cpy(buf, self->buf + self->curr, len);
    self->curr += len;
}

void ipc_unpack_enum(IpcUnpack *self, int *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_size(IpcUnpack *self, size_t *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_ptr(IpcUnpack *self, uintptr_t *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_s8(IpcUnpack *self, int8_t *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_u8(IpcUnpack *self, uint8_t *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_s16(IpcUnpack *self, int16_t *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_u16(IpcUnpack *self, uint16_t *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_s32(IpcUnpack *self, int32_t *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_u32(IpcUnpack *self, uint32_t *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_s64(IpcUnpack *self, int64_t *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_u64(IpcUnpack *self, uint64_t *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_f32(IpcUnpack *self, float *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_f64(IpcUnpack *self, double *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_str(IpcUnpack *self, Str *str)
{
    uint64_t len;
    ipc_unpack_u64(self, &len);
    char *buf = alloc_make_array(self->alloc, char, len);
    ipc_unpack(self, buf, len);
    *str = str_n$(len, buf);
}

void ipc_unpack_cap(IpcUnpack *self, IpcCap *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_proto(IpcUnpack *self, IpcProto *v)
{
    ipc_unpack(self, v, sizeof(*v));
}

void ipc_unpack_slice_impl(IpcUnpack *self, VoidSlice *slice, IpcUnpackFn *el, size_t el_size)
{
    uint64_t len;
    ipc_unpack_u64(self, &len);
    void *buf = alloc_calloc(self->alloc, el_size, len);

    for (uint64_t i = 0; i < len; i++)
    {
        el(self, buf + el_size * i);
    }

    slice->len = len;
    slice->buf = buf;
}
