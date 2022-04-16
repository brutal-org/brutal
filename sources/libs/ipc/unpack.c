#include <ipc/unpack.h>
#include <brutal-debug>

void ipc_unpack_init(IpcUnpack *self, void *buf, size_t len, Alloc *alloc)
{
    *self = (IpcUnpack){
        .buf = buf,
        .len = len,
        .alloc = alloc,
    };
}

void ipc_unpack(IpcUnpack *self, void *buf, size_t len)
{
    mem_cpy(buf, self->buf + self->curr, len);
    self->curr += len;
}

void ipc_unpack_handle(IpcUnpack *self, BrHandle *handle)
{
    assert_lower_than(self->current_handle, BR_MSG_ARG_COUNT - 1);

    ipc_unpack_pod(self, handle);
    *handle = self->handles[self->current_handle++];
}

void ipc_unpack_str(IpcUnpack *self, Str *str)
{
    size_t len;
    ipc_unpack_pod(self, &len);
    char *buf = alloc_make_array(self->alloc, char, len);
    ipc_unpack(self, buf, len);
    *str = str_n$(len, buf);
}

void ipc_unpack_slice_impl(IpcUnpack *self, VoidSlice *slice, IpcUnpackFn *el, size_t el_size)
{
    size_t len;
    ipc_unpack_pod(self, &len);
    void *buf = alloc_calloc(self->alloc, el_size, len);

    for (uint64_t i = 0; i < len; i++)
    {
        el(self, buf + el_size * i);
    }

    slice->len = len;
    slice->buf = buf;
}
