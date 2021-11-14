#pragma once

#include <bal/abi.h>
#include <brutal/ds.h>

typedef struct
{
    void *buf;
    size_t len;
    size_t curr;
    BrMemObj obj;
} BalPack;

typedef void BalPackFn(BalPack *self, void const *buf);

void bal_pack_init(BalPack *self);

void bal_pack_deinit(BalPack *self);

void bal_pack_ensure(BalPack *self, size_t cap);

void bal_pack(BalPack *self, void const *buf, size_t len);

void bal_pack_enum(BalPack *self, int const *s);

void bal_pack_size(BalPack *self, size_t const *s);

void bal_pack_s8(BalPack *self, int8_t const *s);

void bal_pack_u8(BalPack *self, uint8_t const *u);

void bal_pack_s16(BalPack *self, int16_t const *s);

void bal_pack_u16(BalPack *self, uint16_t const *u);

void bal_pack_s32(BalPack *self, int32_t const *s);

void bal_pack_u32(BalPack *self, uint32_t const *u);

void bal_pack_s64(BalPack *self, int64_t const *s);

void bal_pack_u64(BalPack *self, uint64_t const *u);

void bal_pack_f32(BalPack *self, float const *f);

void bal_pack_f64(BalPack *self, double const *f);

void bal_pack_str(BalPack *self, Str const *str);

void bal_pack_slice_impl(BalPack *self, SliceImpl const *v, BalPackFn *el);

#define bal_pack_slice(SELF, SLICE, EL) bal_pack_slice_impl(SELF, &slice_impl$(*(SLICE)), (BalPackFn *)EL)
