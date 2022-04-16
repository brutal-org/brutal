#pragma once

#include <ipc/capability.h>
#include <brutal-ds>
#include <brutal-math>

typedef struct
{
    void *buf;
    size_t len;
    size_t curr;
    Alloc *alloc;

    int current_handle;
    BrHandle handles[BR_MSG_ARG_COUNT - 1];
} IpcUnpack;

typedef void IpcUnpackFn(IpcUnpack *self, void *dest);

void ipc_unpack_init(IpcUnpack *self, void *buf, size_t len, Alloc *alloc);

void ipc_unpack_prepare(IpcUnpack *self, BrMsg *msg);

void ipc_unpack(IpcUnpack *self, void *buf, size_t len);

#define ipc_unpack_pod(SELF, VALUE) ipc_unpack(SELF, VALUE, sizeof(*VALUE))

void ipc_unpack_handle(IpcUnpack *self, BrHandle *handle);

void ipc_unpack_str(IpcUnpack *self, Str *v);

void ipc_unpack_slice_impl(IpcUnpack *self, VoidSlice *slice, IpcUnpackFn *el, size_t el_size);

#define ipc_unpack_slice(SELF, SLICE, EL) ipc_unpack_slice_impl(SELF, (VoidSlice *)(SLICE), (IpcUnpackFn *)(EL), sizeof(*(SLICE)->buf))
