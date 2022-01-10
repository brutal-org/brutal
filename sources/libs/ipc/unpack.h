#pragma once

#include <bal/abi.h>
#include <brutal/ds.h>

typedef struct
{
    void *buf;
    size_t len;
    size_t curr;
    Alloc *alloc;
} IpcUnpack;

typedef void IpcUnpackFn(IpcUnpack *self, void *dest);

void ipc_unpack_init(IpcUnpack *self, void *buf, size_t len, Alloc *alloc);

void ipc_unpack(IpcUnpack *self, void *buf, size_t len);

void ipc_unpack_enum(IpcUnpack *self, int *s);

void ipc_unpack_size(IpcUnpack *self, size_t *s);

void ipc_unpack_s8(IpcUnpack *self, int8_t *s);

void ipc_unpack_u8(IpcUnpack *self, uint8_t *u);

void ipc_unpack_s16(IpcUnpack *self, int16_t *s);

void ipc_unpack_u16(IpcUnpack *self, uint16_t *u);

void ipc_unpack_s32(IpcUnpack *self, int32_t *s);

void ipc_unpack_u32(IpcUnpack *self, uint32_t *u);

void ipc_unpack_s64(IpcUnpack *self, int64_t *s);

void ipc_unpack_u64(IpcUnpack *self, uint64_t *u);

void ipc_unpack_f32(IpcUnpack *self, float *f);

void ipc_unpack_f64(IpcUnpack *self, double *f);

void ipc_unpack_str(IpcUnpack *self, Str *str);

void ipc_unpack_addr(IpcUnpack *self, BrAddr *str);

void ipc_unpack_slice_impl(IpcUnpack *self, VoidSlice *slice, IpcUnpackFn *el, size_t el_size);

#define ipc_unpack_slice(SELF, SLICE, EL) ipc_unpack_slice_impl(SELF, (VoidSlice *)(SLICE), (IpcUnpackFn *)(EL), sizeof(*(SLICE)->buf))
