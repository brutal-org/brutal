#pragma once

#include <brutal/ds.h>
#include <brutal/math.h>
#include <ipc/capability.h>

typedef struct
{
    void *buf;
    size_t len;
    size_t curr;
    BrHandle handle;
} IpcPack;

typedef void IpcPackFn(IpcPack *self, void const *buf);

void ipc_pack_init(IpcPack *self);

void ipc_pack_deinit(IpcPack *self);

void ipc_pack_ensure(IpcPack *self, size_t cap);

void ipc_pack(IpcPack *self, void const *buf, size_t len);

void ipc_pack_enum(IpcPack *self, int const *s);

void ipc_pack_bool(IpcPack *self, bool const *s);

void ipc_pack_size(IpcPack *self, size_t const *s);

void ipc_pack_ptr(IpcPack *self, uintptr_t const *s);

void ipc_pack_s8(IpcPack *self, int8_t const *s);

void ipc_pack_u8(IpcPack *self, uint8_t const *u);

void ipc_pack_s16(IpcPack *self, int16_t const *s);

void ipc_pack_u16(IpcPack *self, uint16_t const *u);

void ipc_pack_s32(IpcPack *self, int32_t const *s);

void ipc_pack_u32(IpcPack *self, uint32_t const *u);

void ipc_pack_s64(IpcPack *self, int64_t const *s);

void ipc_pack_u64(IpcPack *self, uint64_t const *u);

void ipc_pack_f32(IpcPack *self, float const *f);

void ipc_pack_f64(IpcPack *self, double const *f);

void ipc_pack_rect(IpcPack *self, MRect const *v);

void ipc_pack_vec2(IpcPack *self, MVec2 const *v);

void ipc_pack_str(IpcPack *self, Str const *str);

void ipc_pack_cap(IpcPack *self, IpcCap const *addr);

void ipc_pack_slice_impl(IpcPack *self, SliceImpl const *v, IpcPackFn *el);

#define ipc_pack_slice(SELF, SLICE, EL) ipc_pack_slice_impl(SELF, &slice_impl$(*(SLICE)), (IpcPackFn *)EL)
