#pragma once

#include <brutal/ds.h>
#include <brutal/math.h>
#include <ipc/capability.h>

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

void ipc_unpack_enum(IpcUnpack *self, int *v);

void ipc_unpack_bool(IpcUnpack *self, bool *v);

void ipc_unpack_size(IpcUnpack *self, size_t *v);

void ipc_unpack_ptr(IpcUnpack *self, uintptr_t *v);

void ipc_unpack_s8(IpcUnpack *self, int8_t *v);

void ipc_unpack_u8(IpcUnpack *self, uint8_t *v);

void ipc_unpack_s16(IpcUnpack *self, int16_t *v);

void ipc_unpack_u16(IpcUnpack *self, uint16_t *v);

void ipc_unpack_s32(IpcUnpack *self, int32_t *v);

void ipc_unpack_u32(IpcUnpack *self, uint32_t *v);

void ipc_unpack_s64(IpcUnpack *self, int64_t *v);

void ipc_unpack_u64(IpcUnpack *self, uint64_t *v);

void ipc_unpack_f32(IpcUnpack *self, float *v);

void ipc_unpack_f64(IpcUnpack *self, double *v);

void ipc_unpack_rect(IpcUnpack *self, MRect *v);

void ipc_unpack_vec2(IpcUnpack *self, MVec2 *v);

void ipc_unpack_str(IpcUnpack *self, Str *v);

void ipc_unpack_cap(IpcUnpack *self, IpcCap *v);

void ipc_unpack_slice_impl(IpcUnpack *self, VoidSlice *slice, IpcUnpackFn *el, size_t el_size);

#define ipc_unpack_slice(SELF, SLICE, EL) ipc_unpack_slice_impl(SELF, (VoidSlice *)(SLICE), (IpcUnpackFn *)(EL), sizeof(*(SLICE)->buf))
