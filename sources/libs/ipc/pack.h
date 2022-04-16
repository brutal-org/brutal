#pragma once

#include <ipc/capability.h>
#include <brutal-ds>
#include <brutal-math>

typedef struct
{
    void *buf;
    size_t len;
    size_t curr;
    BrHandle handle;

    int handles_count;
    BrHandle handles[BR_MSG_ARG_COUNT - 1];
} IpcPack;

typedef void IpcPackFn(IpcPack *self, void const *buf);

void ipc_pack_init(IpcPack *self);

void ipc_pack_deinit(IpcPack *self);

void ipc_pack_ensure(IpcPack *self, size_t cap);

void ipc_pack(IpcPack *self, void const *buf, size_t len);

#define ipc_pack_pod(SELF, VALUE) ipc_pack(SELF, VALUE, sizeof(*VALUE))

void ipc_pack_handle(IpcPack *self, BrHandle const *handle);

void ipc_pack_str(IpcPack *self, Str const *str);

void ipc_pack_slice_impl(IpcPack *self, SliceImpl const *v, IpcPackFn *el);

#define ipc_pack_slice(SELF, SLICE, EL) ipc_pack_slice_impl(SELF, &slice_impl$(*(SLICE)), (IpcPackFn *)EL)
